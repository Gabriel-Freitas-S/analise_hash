#pragma once
#include <vector>
#include <memory>
#include <stdexcept>

/**
 * @brief Nó da lista encadeada para armazenar valores na tabela hash
 */
struct No {
    int valor;
    std::unique_ptr<No> proximo;
    
    explicit No(int val) : valor(val), proximo(nullptr) {}
};

/**
 * @brief Tabela Hash com tratamento de colisões por encadeamento
 */
class TabelaHashEncadeada {
private:
    std::vector<std::unique_ptr<No>> tabela;
    size_t tamanho;
    size_t numElementos;
    static constexpr double CONSTANTE_MULTIPLICACAO = 0.63274838;
    
public:
    enum class TipoHash { DIVISAO, MULTIPLICACAO };
    
    explicit TabelaHashEncadeada(size_t tam);
    ~TabelaHashEncadeada() = default;
    
    // Prevenir cópia
    TabelaHashEncadeada(const TabelaHashEncadeada&) = delete;
    TabelaHashEncadeada& operator=(const TabelaHashEncadeada&) = delete;
    
    // Permitir movimentação
    TabelaHashEncadeada(TabelaHashEncadeada&&) noexcept = default;
    TabelaHashEncadeada& operator=(TabelaHashEncadeada&&) noexcept = default;
    
    void inserir(int valor, TipoHash tipo);
    bool buscar(int valor, TipoHash tipo) const;
    size_t calcularHashDivisao(int chave) const;
    size_t calcularHashMultiplicacao(int chave) const;
    double fatorCarga() const { return static_cast<double>(numElementos) / tamanho; }
    size_t getNumElementos() const { return numElementos; }
};