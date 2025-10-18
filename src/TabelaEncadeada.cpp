#include "TabelaEncadeada.hpp"
#include <iostream>
#include <algorithm>

void TabelaEncadeada::inserir(int valor, TipoHash tipo) {
    size_t indice = (tipo == TipoHash::DIVISAO) 
        ? calcularHashDivisao(valor) 
        : calcularHashMultiplicacao(valor);
    
    // Verificar se já existe para evitar duplicatas
    if (buscar(valor, tipo)) {
        return; // Valor já existe, não inserir duplicata
    }
    
    // Criar novo nó e inserir no início da lista (mais eficiente)
    auto novoNo = std::make_unique<No>(valor);
    novoNo->proximo = std::move(tabela[indice]);
    tabela[indice] = std::move(novoNo);
    ++numElementos;
}

bool TabelaEncadeada::buscar(int valor, TipoHash tipo) const {
    size_t indice = (tipo == TipoHash::DIVISAO) 
        ? calcularHashDivisao(valor) 
        : calcularHashMultiplicacao(valor);
    
    // Percorrer a lista encadeada na posição calculada
    const No* atual = tabela[indice].get();
    while (atual != nullptr) {
        if (atual->valor == valor) {
            return true;
        }
        atual = atual->proximo.get();
    }
    return false;
}

bool TabelaEncadeada::remover(int valor, TipoHash tipo) {
    size_t indice = (tipo == TipoHash::DIVISAO) 
        ? calcularHashDivisao(valor) 
        : calcularHashMultiplicacao(valor);
    
    // Caso especial: remover o primeiro elemento
    if (tabela[indice] && tabela[indice]->valor == valor) {
        tabela[indice] = std::move(tabela[indice]->proximo);
        --numElementos;
        return true;
    }
    
    // Procurar na lista encadeada
    No* atual = tabela[indice].get();
    while (atual && atual->proximo) {
        if (atual->proximo->valor == valor) {
            // Remover o nó encontrado
            atual->proximo = std::move(atual->proximo->proximo);
            --numElementos;
            return true;
        }
        atual = atual->proximo.get();
    }
    
    return false; // Valor não encontrado
}

TabelaEncadeada::EstatisticasDistribuicao TabelaEncadeada::obterEstatisticas() const {
    EstatisticasDistribuicao stats;
    stats.posicoesMenosUtilizada = 0;
    stats.posicoesMaisUtilizada = 0;
    stats.comprimentoMedio = 0.0;
    stats.totalColisoes = 0;
    
    size_t posicoes_nao_vazias = 0;
    size_t soma_comprimentos = 0;
    
    for (const auto& lista : tabela) {
        size_t comprimento = 0;
        const No* atual = lista.get();
        
        // Contar elementos na lista
        while (atual != nullptr) {
            comprimento++;
            atual = atual->proximo.get();
        }
        
        if (comprimento == 0) {
            stats.posicoesMenosUtilizada++;
        } else {
            posicoes_nao_vazias++;
            soma_comprimentos += comprimento;
            stats.posicoesMaisUtilizada = std::max(stats.posicoesMaisUtilizada, comprimento);
            
            // Cada elemento após o primeiro em uma lista é uma colisão
            if (comprimento > 1) {
                stats.totalColisoes += (comprimento - 1);
            }
        }
    }
    
    // Calcular comprimento médio das listas não vazias
    if (posicoes_nao_vazias > 0) {
        stats.comprimentoMedio = static_cast<double>(soma_comprimentos) / posicoes_nao_vazias;
    }
    
    return stats;
}

// Implementação do operador de saída para TipoHash
std::ostream& operator<<(std::ostream& os, TabelaEncadeada::TipoHash tipo) {
    switch (tipo) {
        case TabelaEncadeada::TipoHash::DIVISAO:
            os << "Divisão";
            break;
        case TabelaEncadeada::TipoHash::MULTIPLICACAO:
            os << "Multiplicação";
            break;
    }
    return os;
}