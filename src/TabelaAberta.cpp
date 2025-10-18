/**
 * @file TabelaAberta.cpp
 * @brief Implementação da classe TabelaAberta
 * 
 * Este arquivo contém a implementação completa de todos os métodos da classe
 * TabelaAberta, incluindo sondagem linear, operações de inserção, busca,
 * remoção e análise de clustering.
 * 
 * @author Gabriel Freitas Souza
 * @author Roberli Schuina Silva
 * @date 2024-10-18
 * @version 1.0
 */

#include "TabelaAberta.hpp"
#include <algorithm>

/**
 * @brief Implementação da sondagem linear
 * 
 * Este é o coração do endereçamento aberto. A sondagem linear incrementa
 * sequencialmente o índice até encontrar uma posição adequada.
 * 
 * Comportamento por tipo de operação:
 * - Inserção: Procura célula VAZIA ou REMOVIDA
 * - Busca: Para em célula VAZIA (não existe) ou encontra o valor
 * 
 * @param indiceInicial Índice calculado pela função hash
 * @param valor Valor sendo manipulado
 * @param paraInsercao Flag indicando o tipo de operação
 * @return Índice válido ou tamanho da tabela se não encontrou
 * 
 * @complexity O(1) média, O(n) no pior caso devido ao clustering
 */
size_t TabelaAberta::sondagemLinear(size_t indiceInicial, int valor, bool paraInsercao) const {
    size_t tentativas = 0;
    size_t indice = indiceInicial;
    
    // Continua sondagem até encontrar posição adequada ou esgotar tentativas
    while (tentativas < tamanho) {
        const Celula& celula = tabela[indice];
        
        if (paraInsercao) {
            // Para inserção: aceita posições VAZIA ou REMOVIDA
            if (celula.disponivelParaInsercao()) {
                return indice;
            }
            // Verifica duplicata: se célula ocupada contém o mesmo valor
            if (celula.estado == Celula::Estado::OCUPADO && celula.valor == valor) {
                return indice; // Retorna posição para tratamento de duplicata
            }
        } else {
            // Para busca: para em célula VAZIA (elemento não existe)
            if (celula.estado == Celula::Estado::VAZIO) {
                return tamanho; // Indica que não foi encontrado
            }
            // Se encontrou o valor em célula ocupada
            if (celula.estado == Celula::Estado::OCUPADO && celula.valor == valor) {
                return indice; // Valor encontrado
            }
            // Continua se for REMOVIDO - pode existir mais adiante
        }
        
        // Avança para próxima posição (sondagem linear)
        indice = (indice + 1) % tamanho;
        ++tentativas;
    }
    
    return tamanho; // Indica que não encontrou posição válida
}

/**
 * @brief Implementação do método de inserção
 * 
 * Processo de inserção:
 * 1. Verifica se precisa de rehash (controle de fator de carga)
 * 2. Calcula índice inicial com função hash
 * 3. Usa sondagem linear para encontrar posição disponível
 * 4. Insere o elemento e atualiza contadores
 * 
 * @param valor Valor a ser inserido
 * @param tipo Função hash a ser utilizada
 * @throws std::runtime_error se tabela cheia ou fator de carga alto
 * 
 * @complexity O(1) amortizada, O(n) no pior caso com clustering severo
 */
void TabelaAberta::inserir(int valor, TipoHash tipo) {
    // Verificação de integridade: evita performance ruim
    if (precisaRehash()) {
        throw std::runtime_error("Fator de carga muito alto - rehash necessário");
    }
    
    // Calcula índice inicial usando função hash especificada
    size_t indiceInicial = (tipo == TipoHash::DIVISAO) 
        ? calcularHashDivisao(valor) 
        : calcularHashMultiplicacao(valor);
    
    // Encontra posição disponível usando sondagem linear
    size_t indice = sondagemLinear(indiceInicial, valor, true);
    
    // Verifica se conseguiu encontrar posição
    if (indice >= tamanho) {
        throw std::runtime_error("Tabela cheia - não foi possível inserir");
    }
    
    Celula& celula = tabela[indice];
    
    // Evita inserção de duplicatas
    if (celula.estado == Celula::Estado::OCUPADO && celula.valor == valor) {
        return; // Valor já existe, não inserir
    }
    
    // Ajusta contadores baseado no estado anterior da célula
    if (celula.estado == Celula::Estado::REMOVIDO) {
        // Reutilizando posição removida
        --numRemovidos;
    }
    
    // Efetua a inserção
    celula.valor = valor;
    celula.estado = Celula::Estado::OCUPADO;
    ++numElementos;
}

/**
 * @brief Implementação do método de busca
 * 
 * Algoritmo de busca:
 * 1. Calcula índice inicial
 * 2. Usa sondagem linear até encontrar valor ou célula vazia
 * 3. Células REMOVIDAS são ignoradas (continua busca)
 * 
 * @param valor Valor a ser buscado
 * @param tipo Função hash utilizada
 * @return true se valor encontrado, false caso contrário
 * 
 * @complexity O(1) média, O(n) no pior caso
 */
bool TabelaAberta::buscar(int valor, TipoHash tipo) const {
    // Calcula índice inicial
    size_t indiceInicial = (tipo == TipoHash::DIVISAO) 
        ? calcularHashDivisao(valor) 
        : calcularHashMultiplicacao(valor);
    
    // Realiza sondagem linear para busca
    size_t indice = sondagemLinear(indiceInicial, valor, false);
    
    // Se índice válido, elemento foi encontrado
    return indice < tamanho;
}

/**
 * @brief Implementação do método de remoção com lazy deletion
 * 
 * A remoção em endereçamento aberto é complexa pois não podemos
 * simplesmente "apagar" um elemento, pois isso quebraria a sondagem
 * linear para elementos inseridos posteriormente. A solução é o
 * "lazy deletion": marcar a célula como REMOVIDA.
 * 
 * @param valor Valor a ser removido
 * @param tipo Função hash utilizada
 * @return Valor removido ou std::nullopt se não encontrado
 * 
 * @complexity O(1) média, O(n) no pior caso
 */
std::optional<int> TabelaAberta::remover(int valor, TipoHash tipo) {
    // Calcula índice inicial
    size_t indiceInicial = (tipo == TipoHash::DIVISAO) 
        ? calcularHashDivisao(valor) 
        : calcularHashMultiplicacao(valor);
    
    // Procura o elemento usando sondagem linear
    size_t indice = sondagemLinear(indiceInicial, valor, false);
    
    // Verifica se elemento foi encontrado
    if (indice >= tamanho) {
        return std::nullopt; // Não encontrado
    }
    
    Celula& celula = tabela[indice];
    
    // Confirma que é o elemento correto em estado ocupado
    if (celula.estado == Celula::Estado::OCUPADO && celula.valor == valor) {
        int valorRemovido = celula.valor;
        
        // Implementa lazy deletion
        celula.marcarRemovido();
        
        // Atualiza contadores
        --numElementos;
        ++numRemovidos;
        
        return valorRemovido;
    }
    
    return std::nullopt; // Não encontrado
}

/**
 * @brief Análise detalhada de clustering e estatísticas de sondagem
 * 
 * Esta função realiza uma análise completa do estado da tabela:
 * 1. Simula operações de busca para calcular sondagens
 * 2. Identifica clusters contíguos de células ocupadas
 * 3. Calcula estatísticas de distribuição
 * 
 * Clustering primário é um problema comum na sondagem linear onde
 * elementos tendem a se agrupar, causando longas seqüências de sondações.
 * 
 * @return Estrutura com estatísticas detalhadas
 * 
 * @complexity O(n) onde n é o tamanho da tabela
 */
TabelaAberta::EstatisticasSondagem TabelaAberta::analisarSondagem() const {
    EstatisticasSondagem stats;
    stats.totalSondagens = 0;
    stats.sondagemMedia = 0.0;
    stats.maxSondagens = 0;
    stats.clustersDetectados = 0;
    stats.maiorCluster = 0;
    
    // Se tabela vazia, retorna estatísticas zeradas
    if (numElementos == 0) {
        return stats;
    }
    
    // Simula operações de busca para calcular sondagens
    size_t totalSondagens = 0;
    size_t operacoesRealizadas = 0;
    
    // Para cada elemento ocupado, calcula quantas sondagens seriam necessárias
    for (size_t i = 0; i < tamanho; ++i) {
        if (tabela[i].estado == Celula::Estado::OCUPADO) {
            int valor = tabela[i].valor;
            
            // Simula busca para calcular número de sondagens
            size_t indiceOriginal = calcularHashDivisao(valor);
            size_t sondagens = 1;
            size_t indice = indiceOriginal;
            
            // Conta sondagens até encontrar a posição atual
            while (indice != i) {
                indice = (indice + 1) % tamanho;
                ++sondagens;
                
                // Proteção contra loop infinito
                if (sondagens > tamanho) break;
            }
            
            totalSondagens += sondagens;
            stats.maxSondagens = std::max(stats.maxSondagens, sondagens);
            ++operacoesRealizadas;
        }
    }
    
    // Calcula média de sondagens
    if (operacoesRealizadas > 0) {
        stats.sondagemMedia = static_cast<double>(totalSondagens) / operacoesRealizadas;
    }
    stats.totalSondagens = totalSondagens;
    
    // Análise de clustering
    bool emCluster = false;
    size_t tamanhoClusterAtual = 0;
    
    // Percorre a tabela identificando clusters contíguos
    for (size_t i = 0; i < tamanho; ++i) {
        if (tabela[i].estado != Celula::Estado::VAZIO) {
            if (!emCluster) {
                // Início de novo cluster
                emCluster = true;
                tamanhoClusterAtual = 1;
            } else {
                // Continuação do cluster atual
                ++tamanhoClusterAtual;
            }
        } else {
            if (emCluster) {
                // Fim do cluster - registra se significativo
                if (tamanhoClusterAtual > 1) {
                    ++stats.clustersDetectados;
                    stats.maiorCluster = std::max(stats.maiorCluster, tamanhoClusterAtual);
                }
                emCluster = false;
                tamanhoClusterAtual = 0;
            }
        }
    }
    
    // Trata caso especial: cluster que "dá a volta" na tabela
    // (termina no final e continua no início)
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
        // Cluster que termina no final da tabela
        ++stats.clustersDetectados;
        stats.maiorCluster = std::max(stats.maiorCluster, tamanhoClusterAtual);
    }
    
    return stats;
}

/**
 * @brief Operador de saída para TipoHash
 * 
 * Converte o tipo enum para string legível.
 * 
 * @param os Stream de saída
 * @param tipo Tipo a ser convertido
 * @return Referência ao stream para encadeamento
 */
std::ostream& operator<<(std::ostream& os, TabelaAberta::TipoHash tipo) {
    switch (tipo) {
        case TabelaAberta::TipoHash::DIVISAO:
            os << "Divisão";
            break;
        case TabelaAberta::TipoHash::MULTIPLICACAO:
            os << "Multiplicação";
            break;
        default:
            os << "Desconhecido";
            break;
    }
    return os;
}

/**
 * @brief Operador de saída para Estado da Célula
 * 
 * Converte o estado da célula para string legível.
 * Útil para debugging e análise da tabela.
 * 
 * @param os Stream de saída
 * @param estado Estado a ser convertido
 * @return Referência ao stream para encadeamento
 */
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
        default:
            os << "Inválido";
            break;
    }
    return os;
}