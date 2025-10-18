#pragma once

#include <vector>
#include <memory>
#include <stdexcept>
#include <cmath>

/**
 * @brief Nó da lista encadeada para armazenar valores na tabela hash
 */
struct No {
    int valor;                      ///< Valor armazenado no nó
    std::unique_ptr<No> proximo;    ///< Ponteiro para o próximo nó
    
    /**
     * @brief Construtor do nó
     * @param val Valor a ser armazenado
     */
    explicit No(int val) : valor(val), proximo(nullptr) {}
};

/**
 * @brief Tabela Hash com tratamento de colisões por encadeamento (chaining)
 * 
 * Esta implementação utiliza listas encadeadas para resolver colisões,
 * onde cada posição da tabela contém uma lista de elementos que
 * foram mapeados para o mesmo índice.
 * 
 * Suporta duas funções de hash:
 * - Método da Divisão: h(k) = k mod p
 * - Método da Multiplicação: h(k) = floor((k * c mod 1) * p)
 */
class TabelaEncadeada {
private:
    std::vector<std::unique_ptr<No>> tabela;    ///< Tabela de ponteiros para listas
    size_t tamanho;                             ///< Tamanho da tabela
    size_t numElementos;                        ///< Número de elementos inseridos
    
    /// Constante para o método da multiplicação (Fração áurea)
    static constexpr double CONSTANTE_MULTIPLICACAO = 0.6180339887;
    
    /**
     * @brief Verifica se um número é primo
     * @param n Número a ser verificado
     * @return true se o número for primo
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
     */
    enum class TipoHash { 
        DIVISAO,        ///< Método da divisão
        MULTIPLICACAO   ///< Método da multiplicação
    };
    
    /**
     * @brief Construtor da tabela hash com encadeamento
     * @param tam Tamanho da tabela (recomendável usar número primo)
     * @throws std::invalid_argument se o tamanho for zero
     */
    explicit TabelaEncadeada(size_t tam) : tamanho(tam), numElementos(0) {
        if (tam == 0) {
            throw std::invalid_argument("Tamanho da tabela deve ser maior que zero");
        }
        
        // Aviso se o tamanho não for primo (melhor distribuição)
        if (!ehPrimo(tam)) {
            // Para fins de performance, apenas registramos internamente
            // sem imprimir warnings durante benchmarks
        }
        
        tabela.resize(tamanho);
    }
    
    /**
     * @brief Destrutor padrão
     */
    ~TabelaEncadeada() = default;
    
    // Prevenir cópia (sem necessidade e custosa)
    TabelaEncadeada(const TabelaEncadeada&) = delete;
    TabelaEncadeada& operator=(const TabelaEncadeada&) = delete;
    
    // Permitir movimentação
    TabelaEncadeada(TabelaEncadeada&&) noexcept = default;
    TabelaEncadeada& operator=(TabelaEncadeada&&) noexcept = default;
    
    /**
     * @brief Insere um valor na tabela hash
     * @param valor Valor a ser inserido
     * @param tipo Tipo da função hash a ser utilizada
     * @note Não permite duplicatas
     */
    void inserir(int valor, TipoHash tipo);
    
    /**
     * @brief Busca um valor na tabela hash
     * @param valor Valor a ser buscado
     * @param tipo Tipo da função hash utilizada na inserção
     * @return true se o valor foi encontrado, false caso contrário
     */
    bool buscar(int valor, TipoHash tipo) const;
    
    /**
     * @brief Remove um valor da tabela hash
     * @param valor Valor a ser removido
     * @param tipo Tipo da função hash utilizada
     * @return true se o valor foi removido, false se não estava presente
     */
    bool remover(int valor, TipoHash tipo);
    
    /**
     * @brief Calcula o índice usando o método da divisão
     * @param chave Chave a ser mapeada
     * @return Índice na tabela
     */
    size_t calcularHashDivisao(int chave) const {
        return static_cast<size_t>(std::abs(chave)) % tamanho;
    }
    
    /**
     * @brief Calcula o índice usando o método da multiplicação
     * @param chave Chave a ser mapeada
     * @return Índice na tabela
     */
    size_t calcularHashMultiplicacao(int chave) const {
        double produto = std::abs(chave) * CONSTANTE_MULTIPLICACAO;
        double fracao = produto - std::floor(produto);
        return static_cast<size_t>(std::floor(fracao * tamanho));
    }
    
    /**
     * @brief Calcula o fator de carga atual da tabela
     * @return Fator de carga (elementos / tamanho da tabela)
     */
    double fatorCarga() const { 
        return static_cast<double>(numElementos) / tamanho; 
    }
    
    /**
     * @brief Obtém o número de elementos inseridos
     * @return Número de elementos
     */
    size_t getNumElementos() const { 
        return numElementos; 
    }
    
    /**
     * @brief Obtém o tamanho da tabela
     * @return Tamanho da tabela
     */
    size_t getTamanho() const { 
        return tamanho; 
    }
    
    /**
     * @brief Verifica se a tabela está vazia
     * @return true se não houver elementos
     */
    bool vazia() const { 
        return numElementos == 0; 
    }
    
    /**
     * @brief Limpa todos os elementos da tabela
     */
    void limpar() {
        for (auto& lista : tabela) {
            lista.reset();
        }
        numElementos = 0;
    }
    
    /**
     * @brief Obtém estatísticas da distribuição dos elementos
     * @return Estrutura com informações sobre a distribuição
     */
    struct EstatisticasDistribuicao {
        size_t posicoesMenosUtilizada;  ///< Número de posições vazias
        size_t posicoesMaisUtilizada;   ///< Tamanho da maior lista encadeada
        double comprimentoMedio;       ///< Comprimento médio das listas não vazias
        size_t totalColisoes;          ///< Número estimado de colisões
    };
    
    /**
     * @brief Calcula estatísticas sobre a distribuição dos elementos
     * @return Estrutura com as estatísticas
     */
    EstatisticasDistribuicao obterEstatisticas() const;
};

/**
 * @brief Overload do operador de saída para TipoHash
 * @param os Stream de saída
 * @param tipo Tipo de hash a ser impresso
 * @return Referência para o stream
 */
std::ostream& operator<<(std::ostream& os, TabelaEncadeada::TipoHash tipo);