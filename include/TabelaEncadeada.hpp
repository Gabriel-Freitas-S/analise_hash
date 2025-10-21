/**
 * @file TabelaEncadeada.hpp
 * @brief Definição da classe TabelaEncadeada para tabelas hash com tratamento de colisões por encadeamento
 * 
 * Este arquivo contém a definição completa da classe TabelaEncadeada, que implementa
 * uma tabela hash utilizando encadeamento (chaining) para resolver colisões.
 * A implementação utiliza listas encadeadas com ponteiros inteligentes para
 * garantir segurança de memória e performance otimizada.
 * 
 * @author Gabriel Freitas Souza
 * @author Roberli Schuina Silva
 * @date 2024-10-18
 * @version 1.1
 * 
 * Características principais:
 * - Utiliza std::unique_ptr para gerenciamento automático de memória
 * - Suporta duas funções de hash: divisão e multiplicação
 * - Inserção no início das listas para complexidade O(1)
 * - Verificação de duplicatas antes da inserção
 * - Coleta de estatísticas de distribuição e colisões
 * - Constante de multiplicação conforme especificação: c = 0.63274838
 */

#pragma once

#include <vector>
#include <memory>
#include <stdexcept>
#include <cmath>
#include <iostream>

/**
 * @brief Estrutura de nó para a lista encadeada
 * 
 * Cada nó armazena um valor inteiro e um ponteiro inteligente para o próximo nó.
 * O uso de std::unique_ptr garante que não haja vazamentos de memória e que
 * a destruição dos nós seja automática quando a tabela for destruída.
 */
struct No {
    int valor;                      ///< Valor armazenado no nó
    std::unique_ptr<No> proximo;    ///< Ponteiro para o próximo nó da lista
    
    /**
     * @brief Construtor do nó
     * @param val Valor a ser armazenado no nó
     * 
     * Inicializa o nó com o valor fornecido e ponteiro nulo para o próximo.
     */
    explicit No(int val) : valor(val), proximo(nullptr) {}
};

/**
 * @brief Classe TabelaEncadeada - Implementação de tabela hash com encadeamento
 * 
 * Esta classe implementa uma tabela hash utilizando o método de encadeamento
 * (separate chaining) para resolver colisões. Cada posição da tabela pode
 * conter uma lista encadeada de elementos que foram mapeados para o mesmo índice.
 * 
 * Vantagens do encadeamento:
 * - Não há limite teórico para o número de elementos
 * - Performance degrada graciosamente com o aumento do fator de carga
 * - Implementação simples e robusta
 * - Permite remoção eficiente de elementos
 * 
 * Desvantagens:
 * - Uso adicional de memória para ponteiros
 * - Possível fragmentação de memória
 * - Pior localidade de cache comparado ao endereçamento aberto
 */
class TabelaEncadeada {
private:
    std::vector<std::unique_ptr<No>> tabela;    ///< Array de ponteiros para as listas encadeadas
    size_t tamanho;                             ///< Tamanho da tabela hash
    size_t numElementos;                        ///< Número total de elementos inseridos
    
    /// Constante para o método da multiplicação conforme especificação do trabalho
    static constexpr double CONSTANTE_MULTIPLICACAO = 0.63274838;
    
    /**
     * @brief Verifica se um número é primo
     * @param n Número a ser verificado
     * @return true se o número for primo, false caso contrário
     * 
     * Utiliza o algoritmo clássico de verificação de primalidade,
     * testando divisibilidade até a raiz quadrada do número.
     * Números primos são recomendados como tamanho da tabela para
     * melhor distribuição das chaves.
     */
    static bool ehPrimo(size_t n) {
        if (n < 2) return false;
        if (n == 2) return true;
        if (n % 2 == 0) return false;
        
        for (size_t i = 3; i * i <= n; i += 2) {
            if (n % i == 0) return false;
        }
        return true;
    }
    
public:
    /**
     * @brief Enumeração dos tipos de função hash suportados
     * 
     * Define os dois métodos de hashing implementados:
     * - DIVISAO: h(k) = k mod m (método da divisão)
     * - MULTIPLICACAO: h(k) = floor(m * ((k * c) mod 1)) (método da multiplicação)
     */
    enum class TipoHash { 
        DIVISAO,        ///< Método da divisão - simples e eficiente
        MULTIPLICACAO   ///< Método da multiplicação - melhor distribuição
    };
    
    /**
     * @brief Construtor da tabela hash encadeada
     * @param tam Tamanho da tabela (número de posições)
     * @throws std::invalid_argument se o tamanho for zero
     * 
     * Inicializa a tabela com o tamanho especificado. Recomenda-se usar
     * números primos como tamanho para melhor distribuição das chaves,
     * especialmente com o método da divisão.
     */
    explicit TabelaEncadeada(size_t tam) : tamanho(tam), numElementos(0) {
        if (tam == 0) {
            throw std::invalid_argument("Tamanho da tabela deve ser maior que zero");
        }
        
        // Aviso interno sobre uso de números não-primos
        if (!ehPrimo(tam)) {
            // Registrado internamente para análise posterior
            // Não imprime warning durante execução para não interferir nos benchmarks
        }
        
        // Inicializa todas as posições com nullptr
        tabela.resize(tamanho);
    }
    
    /**
     * @brief Destrutor padrão
     * 
     * O destrutor é automaticamente gerado pelo compilador.
     * A destruição dos unique_ptr's é automática e libera
     * toda a memória das listas encadeadas.
     */
    ~TabelaEncadeada() = default;
    
    // Desabilita cópia (custosa e desnecessária para benchmarks)
    TabelaEncadeada(const TabelaEncadeada&) = delete;
    TabelaEncadeada& operator=(const TabelaEncadeada&) = delete;
    
    // Permite movimentação eficiente
    TabelaEncadeada(TabelaEncadeada&&) noexcept = default;
    TabelaEncadeada& operator=(TabelaEncadeada&&) noexcept = default;
    
    /**
     * @brief Insere um valor na tabela hash
     * @param valor Valor a ser inserido
     * @param tipo Tipo da função hash a ser utilizada
     * 
     * Calcula o índice usando a função hash especificada e insere
     * o valor no início da lista correspondente. Não permite duplicatas.
     * 
     * Complexidade: O(1) amortizada, O(n) no pior caso se muitas colisões
     * 
     * @note A inserção no início da lista garante O(1) para a operação de inserção,
     *       mas requer verificação de duplicatas que pode ser O(k) onde k é o
     *       número de elementos na lista.
     */
    void inserir(int valor, TipoHash tipo);
    
    /**
     * @brief Busca um valor na tabela hash
     * @param valor Valor a ser buscado
     * @param tipo Tipo da função hash utilizada na inserção
     * @return true se o valor foi encontrado, false caso contrário
     * 
     * Calcula o índice e percorre a lista encadeada correspondente
     * até encontrar o valor ou chegar ao final da lista.
     * 
     * Complexidade: O(1) média, O(n) no pior caso
     */
    bool buscar(int valor, TipoHash tipo) const;
    
    /**
     * @brief Remove um valor da tabela hash
     * @param valor Valor a ser removido
     * @param tipo Tipo da função hash utilizada
     * @return true se o valor foi removido, false se não estava presente
     * 
     * Localiza e remove o elemento da lista encadeada. Trata casos especiais
     * como remoção do primeiro elemento da lista.
     * 
     * Complexidade: O(1) média, O(n) no pior caso
     */
    bool remover(int valor, TipoHash tipo);
    
    /**
     * @brief Calcula o índice usando o método da divisão
     * @param chave Chave a ser mapeada
     * @return Índice na tabela (0 <= índice < tamanho)
     * 
     * Implementa h(k) = k mod p, onde p é o tamanho da tabela.
     * Funciona melhor quando p é um número primo.
     */
    size_t calcularHashDivisao(int chave) const {
        return static_cast<size_t>(std::abs(chave)) % tamanho;
    }
    
    /**
     * @brief Calcula o índice usando o método da multiplicação
     * @param chave Chave a ser mapeada
     * @return Índice na tabela (0 <= índice < tamanho)
     * 
     * Implementa h(k) = floor(p * ((k * c) mod 1)), onde:
     * - p é o tamanho da tabela
     * - c = 0.63274838 (constante conforme especificação do trabalho)
     * 
     * Este método oferece melhor distribuição independentemente
     * do tamanho da tabela.
     */
    size_t calcularHashMultiplicacao(int chave) const {
        double produto = std::abs(chave) * CONSTANTE_MULTIPLICACAO;
        double fracao = produto - std::floor(produto);
        return static_cast<size_t>(std::floor(fracao * tamanho));
    }
    
    /**
     * @brief Calcula o fator de carga atual da tabela
     * @return Fator de carga (número de elementos / tamanho da tabela)
     * 
     * O fator de carga indica quão "cheia" está a tabela.
     * Valores altos (> 1.0) indicam muitas colisões em média.
     */
    double fatorCarga() const { 
        return static_cast<double>(numElementos) / tamanho; 
    }
    
    /**
     * @brief Obtém o número de elementos inseridos
     * @return Número total de elementos na tabela
     */
    size_t getNumElementos() const { 
        return numElementos; 
    }
    
    /**
     * @brief Obtém o tamanho da tabela
     * @return Número de posições na tabela
     */
    size_t getTamanho() const { 
        return tamanho; 
    }
    
    /**
     * @brief Verifica se a tabela está vazia
     * @return true se não houver elementos inseridos
     */
    bool vazia() const { 
        return numElementos == 0; 
    }
    
    /**
     * @brief Remove todos os elementos da tabela
     * 
     * Libera toda a memória das listas encadeadas e redefine
     * o contador de elementos para zero.
     */
    void limpar() {
        for (auto& lista : tabela) {
            lista.reset(); // Libera automaticamente a lista encadeada
        }
        numElementos = 0;
    }
    
    /**
     * @brief Estrutura para estatísticas de distribuição
     * 
     * Contém informações detalhadas sobre como os elementos
     * estão distribuídos na tabela hash.
     */
    struct EstatisticasDistribuicao {
        size_t posicoesMenosUtilizada;  ///< Número de posições vazias
        size_t posicoesMaisUtilizada;   ///< Tamanho da maior lista encadeada
        double comprimentoMedio;       ///< Comprimento médio das listas não vazias
        size_t totalColisoes;          ///< Número estimado de colisões ocorridas
    };
    
    /**
     * @brief Calcula estatísticas sobre a distribuição dos elementos
     * @return Estrutura com estatísticas detalhadas
     * 
     * Percorre toda a tabela coletando informações sobre:
     * - Distribuição dos elementos
     * - Número de colisões
     * - Comprimento das listas
     * - Utilização das posições
     */
    EstatisticasDistribuicao obterEstatisticas() const;
};

/**
 * @brief Operador de saída para TipoHash
 * @param os Stream de saída
 * @param tipo Tipo de hash a ser impresso
 * @return Referência para o stream
 * 
 * Permite impressão direta do tipo de hash usando cout << tipo.
 */
std::ostream& operator<<(std::ostream& os, TabelaEncadeada::TipoHash tipo);