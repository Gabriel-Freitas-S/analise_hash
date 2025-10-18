/**
 * @file TabelaEncadeada.cpp
 * @brief Implementação da classe TabelaEncadeada
 * 
 * Este arquivo contém a implementação completa de todos os métodos da classe
 * TabelaEncadeada, incluindo operações de inserção, busca, remoção e
 * coleta de estatísticas.
 * 
 * @author Gabriel Freitas Souza
 * @author Roberli Schuina Silva
 * @date 2024-10-18
 * @version 1.0
 */

#include "TabelaEncadeada.hpp"
#include <iostream>
#include <algorithm>

/**
 * @brief Implementação do método de inserção
 * 
 * Esta função implementa a inserção de elementos na tabela hash com encadeamento.
 * O processo segue os seguintes passos:
 * 1. Calcula o índice usando a função hash especificada
 * 2. Verifica se o elemento já existe para evitar duplicatas
 * 3. Cria um novo nó e o insere no início da lista (mais eficiente)
 * 4. Atualiza o contador de elementos
 * 
 * @param valor Valor a ser inserido na tabela
 * @param tipo Função hash a ser utilizada (DIVISAO ou MULTIPLICACAO)
 * 
 * @complexity O(1) amortizada para inserção, O(k) para verificação de duplicatas,
 *             onde k é o número de elementos na lista da posição calculada
 */
void TabelaEncadeada::inserir(int valor, TipoHash tipo) {
    // Calcula o índice baseado na função hash escolhida
    size_t indice = (tipo == TipoHash::DIVISAO) 
        ? calcularHashDivisao(valor) 
        : calcularHashMultiplicacao(valor);
    
    // Verifica se o valor já existe na lista para evitar duplicatas
    // Esta verificação é importante para manter a integridade dos dados
    if (buscar(valor, tipo)) {
        return; // Elemento já existe, não inserir duplicata
    }
    
    // Cria um novo nó e o insere no início da lista
    // Inserção no início é O(1) e não requer percorrer a lista
    auto novoNo = std::make_unique<No>(valor);
    novoNo->proximo = std::move(tabela[indice]);
    tabela[indice] = std::move(novoNo);
    
    // Incrementa o contador de elementos
    ++numElementos;
}

/**
 * @brief Implementação do método de busca
 * 
 * Realiza a busca de um elemento na tabela hash. O algoritmo:
 * 1. Calcula o índice usando a mesma função hash da inserção
 * 2. Percorre sequencialmente a lista encadeada na posição calculada
 * 3. Retorna true se encontrar o elemento, false caso contrário
 * 
 * @param valor Valor a ser buscado
 * @param tipo Função hash utilizada (deve ser a mesma da inserção)
 * @return true se o elemento foi encontrado, false caso contrário
 * 
 * @complexity O(1) média, O(n) no pior caso onde n é o número de elementos
 *             na lista da posição calculada
 */
bool TabelaEncadeada::buscar(int valor, TipoHash tipo) const {
    // Calcula o índice usando a mesma função hash
    size_t indice = (tipo == TipoHash::DIVISAO) 
        ? calcularHashDivisao(valor) 
        : calcularHashMultiplicacao(valor);
    
    // Percorre a lista encadeada na posição calculada
    const No* atual = tabela[indice].get();
    while (atual != nullptr) {
        if (atual->valor == valor) {
            return true; // Elemento encontrado
        }
        atual = atual->proximo.get();
    }
    
    return false; // Elemento não encontrado
}

/**
 * @brief Implementação do método de remoção
 * 
 * Remove um elemento da tabela hash. O algoritmo trata dois casos:
 * 1. Remoção do primeiro elemento da lista (caso especial)
 * 2. Remoção de elemento no meio/final da lista
 * 
 * @param valor Valor a ser removido
 * @param tipo Função hash utilizada
 * @return true se o elemento foi removido, false se não estava presente
 * 
 * @complexity O(1) média, O(n) no pior caso
 */
bool TabelaEncadeada::remover(int valor, TipoHash tipo) {
    // Calcula o índice usando a função hash
    size_t indice = (tipo == TipoHash::DIVISAO) 
        ? calcularHashDivisao(valor) 
        : calcularHashMultiplicacao(valor);
    
    // Caso especial: remover o primeiro elemento da lista
    if (tabela[indice] && tabela[indice]->valor == valor) {
        tabela[indice] = std::move(tabela[indice]->proximo);
        --numElementos;
        return true;
    }
    
    // Procurar o elemento na lista encadeada
    No* atual = tabela[indice].get();
    while (atual && atual->proximo) {
        if (atual->proximo->valor == valor) {
            // Remove o nó encontrado reconectando os ponteiros
            atual->proximo = std::move(atual->proximo->proximo);
            --numElementos;
            return true;
        }
        atual = atual->proximo.get();
    }
    
    return false; // Valor não encontrado para remoção
}

/**
 * @brief Coleta estatísticas detalhadas sobre a distribuição dos elementos
 * 
 * Percorre toda a tabela hash coletando informações estatísticas sobre:
 * - Número de posições vazias e ocupadas
 * - Tamanho da maior lista encadeada
 * - Comprimento médio das listas não vazias
 * - Estimativa do número total de colisões
 * 
 * @return Estrutura EstatisticasDistribuicao com as métricas coletadas
 * 
 * @complexity O(n + m) onde n é o número de elementos e m o tamanho da tabela
 */
TabelaEncadeada::EstatisticasDistribuicao TabelaEncadeada::obterEstatisticas() const {
    EstatisticasDistribuicao stats;
    stats.posicoesMenosUtilizada = 0;  // Inicialmente, nenhuma posição vazia contada
    stats.posicoesMaisUtilizada = 0;   // Maior lista encontrada
    stats.comprimentoMedio = 0.0;      // Média das listas não vazias
    stats.totalColisoes = 0;           // Total de colisões estimadas
    
    size_t posicoes_nao_vazias = 0;    // Contador de posições com elementos
    size_t soma_comprimentos = 0;      // Soma dos comprimentos para cálculo da média
    
    // Percorre todas as posições da tabela
    for (const auto& lista : tabela) {
        size_t comprimento = 0;
        const No* atual = lista.get();
        
        // Conta elementos na lista encadeada desta posição
        while (atual != nullptr) {
            comprimento++;
            atual = atual->proximo.get();
        }
        
        if (comprimento == 0) {
            // Posição vazia
            stats.posicoesMenosUtilizada++;
        } else {
            // Posição ocupada - atualiza estatísticas
            posicoes_nao_vazias++;
            soma_comprimentos += comprimento;
            stats.posicoesMaisUtilizada = std::max(stats.posicoesMaisUtilizada, comprimento);
            
            // Cada elemento após o primeiro em uma lista representa uma colisão
            if (comprimento > 1) {
                stats.totalColisoes += (comprimento - 1);
            }
        }
    }
    
    // Calcula o comprimento médio das listas não vazias
    if (posicoes_nao_vazias > 0) {
        stats.comprimentoMedio = static_cast<double>(soma_comprimentos) / posicoes_nao_vazias;
    }
    
    return stats;
}

/**
 * @brief Implementação do operador de saída para TipoHash
 * 
 * Permite a impressão direta do tipo de função hash usando streams.
 * Útil para logs, debugging e relatórios.
 * 
 * @param os Stream de saída
 * @param tipo Tipo de hash a ser impresso
 * @return Referência para o stream (permite encadeamento)
 */
std::ostream& operator<<(std::ostream& os, TabelaEncadeada::TipoHash tipo) {
    switch (tipo) {
        case TabelaEncadeada::TipoHash::DIVISAO:
            os << "Divisão";
            break;
        case TabelaEncadeada::TipoHash::MULTIPLICACAO:
            os << "Multiplicação";
            break;
        default:
            os << "Desconhecido";
            break;
    }
    return os;
}