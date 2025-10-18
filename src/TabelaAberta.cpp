#include "TabelaAberta.hpp"
#include <algorithm>

size_t TabelaAberta::sondagemLinear(size_t indiceInicial, int valor, bool paraInsercao) const {
    size_t tentativas = 0;
    size_t indice = indiceInicial;
    
    while (tentativas < tamanho) {
        const Celula& celula = tabela[indice];
        
        if (paraInsercao) {
            // Para inserção: aceita VAZIO ou REMOVIDO
            if (celula.disponivelParaInsercao()) {
                return indice;
            }
            // Se a célula está ocupada com o mesmo valor, retorna para evitar duplicatas
            if (celula.estado == Celula::Estado::OCUPADO && celula.valor == valor) {
                return indice;
            }
        } else {
            // Para busca: para em VAZIO ou encontra o valor
            if (celula.estado == Celula::Estado::VAZIO) {
                return tamanho; // Não encontrado
            }
            if (celula.estado == Celula::Estado::OCUPADO && celula.valor == valor) {
                return indice; // Encontrado
            }
            // Continua se for REMOVIDO
        }
        
        indice = (indice + 1) % tamanho;
        ++tentativas;
    }
    
    return tamanho; // Indica que não encontrou posição válida
}

void TabelaAberta::inserir(int valor, TipoHash tipo) {
    // Verificar se precisa de rehash
    if (precisaRehash()) {
        throw std::runtime_error("Fator de carga muito alto - rehash necessário");
    }
    
    size_t indiceInicial = (tipo == TipoHash::DIVISAO) 
        ? calcularHashDivisao(valor) 
        : calcularHashMultiplicacao(valor);
    
    size_t indice = sondagemLinear(indiceInicial, valor, true);
    
    if (indice >= tamanho) {
        throw std::runtime_error("Tabela cheia - não foi possível inserir");
    }
    
    Celula& celula = tabela[indice];
    
    // Não inserir duplicatas
    if (celula.estado == Celula::Estado::OCUPADO && celula.valor == valor) {
        return; // Valor já existe
    }
    
    // Se a posição estava marcada como removida, decrementar contador
    if (celula.estado == Celula::Estado::REMOVIDO) {
        --numRemovidos;
    }
    
    // Inserir o valor
    celula.valor = valor;
    celula.estado = Celula::Estado::OCUPADO;
    ++numElementos;
}

bool TabelaAberta::buscar(int valor, TipoHash tipo) const {
    size_t indiceInicial = (tipo == TipoHash::DIVISAO) 
        ? calcularHashDivisao(valor) 
        : calcularHashMultiplicacao(valor);
    
    size_t indice = sondagemLinear(indiceInicial, valor, false);
    return indice < tamanho;
}

std::optional<int> TabelaAberta::remover(int valor, TipoHash tipo) {
    size_t indiceInicial = (tipo == TipoHash::DIVISAO) 
        ? calcularHashDivisao(valor) 
        : calcularHashMultiplicacao(valor);
    
    size_t indice = sondagemLinear(indiceInicial, valor, false);
    
    if (indice >= tamanho) {
        return std::nullopt; // Não encontrado
    }
    
    Celula& celula = tabela[indice];
    if (celula.estado == Celula::Estado::OCUPADO && celula.valor == valor) {
        int valorRemovido = celula.valor;
        celula.marcarRemovido();
        --numElementos;
        ++numRemovidos;
        return valorRemovido;
    }
    
    return std::nullopt; // Não encontrado
}

TabelaAberta::EstatisticasSondagem TabelaAberta::analisarSondagem() const {
    EstatisticasSondagem stats;
    stats.totalSondagens = 0;
    stats.sondagemMedia = 0.0;
    stats.maxSondagens = 0;
    stats.clustersDetectados = 0;
    stats.maiorCluster = 0;
    
    if (numElementos == 0) {
        return stats;
    }
    
    // Simular buscas para calcular estatísticas de sondagem
    size_t totalSondagens = 0;
    size_t operacoesRealizadas = 0;
    
    for (size_t i = 0; i < tamanho; ++i) {
        if (tabela[i].estado == Celula::Estado::OCUPADO) {
            int valor = tabela[i].valor;
            
            // Calcular quantas sondagens seriam necessárias para encontrar este elemento
            size_t indiceOriginal = calcularHashDivisao(valor); // Usar uma função padrão
            size_t sondagens = 1;
            size_t indice = indiceOriginal;
            
            while (indice != i) {
                indice = (indice + 1) % tamanho;
                ++sondagens;
                if (sondagens > tamanho) break; // Proteção contra loop infinito
            }
            
            totalSondagens += sondagens;
            stats.maxSondagens = std::max(stats.maxSondagens, sondagens);
            ++operacoesRealizadas;
        }
    }
    
    if (operacoesRealizadas > 0) {
        stats.sondagemMedia = static_cast<double>(totalSondagens) / operacoesRealizadas;
    }
    stats.totalSondagens = totalSondagens;
    
    // Analisar clustering
    bool emCluster = false;
    size_t tamanhoClusterAtual = 0;
    
    for (size_t i = 0; i < tamanho; ++i) {
        if (tabela[i].estado != Celula::Estado::VAZIO) {
            if (!emCluster) {
                emCluster = true;
                tamanhoClusterAtual = 1;
            } else {
                ++tamanhoClusterAtual;
            }
        } else {
            if (emCluster) {
                if (tamanhoClusterAtual > 1) {
                    ++stats.clustersDetectados;
                    stats.maiorCluster = std::max(stats.maiorCluster, tamanhoClusterAtual);
                }
                emCluster = false;
                tamanhoClusterAtual = 0;
            }
        }
    }
    
    // Verificar se há um cluster no final que se conecta com o início
    if (emCluster && tamanho > 0 && tabela[0].estado != Celula::Estado::VAZIO) {
        size_t inicio = 0;
        while (inicio < tamanho && tabela[inicio].estado != Celula::Estado::VAZIO) {
            ++tamanhoClusterAtual;
            ++inicio;
        }
        if (tamanhoClusterAtual > 1) {
            ++stats.clustersDetectados;
            stats.maiorCluster = std::max(stats.maiorCluster, tamanhoClusterAtual);
        }
    } else if (emCluster && tamanhoClusterAtual > 1) {
        ++stats.clustersDetectados;
        stats.maiorCluster = std::max(stats.maiorCluster, tamanhoClusterAtual);
    }
    
    return stats;
}

// Implementações dos operadores de saída
std::ostream& operator<<(std::ostream& os, TabelaAberta::TipoHash tipo) {
    switch (tipo) {
        case TabelaAberta::TipoHash::DIVISAO:
            os << "Divisão";
            break;
        case TabelaAberta::TipoHash::MULTIPLICACAO:
            os << "Multiplicação";
            break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, Celula::Estado estado) {
    switch (estado) {
        case Celula::Estado::VAZIO:
            os << "Vazio";
            break;
        case Celula::Estado::OCUPADO:
            os << "Ocupado";
            break;
        case Celula::Estado::REMOVIDO:
            os << "Removido";
            break;
    }
    return os;
}