#pragma once
#include <vector>
#include <optional>
#include <stdexcept>

/**
 * @brief Célula da tabela hash com endereçamento aberto
 */
struct Celula {
    enum class Estado { VAZIO, OCUPADO, REMOVIDO };
    
    int valor;
    Estado estado;
    
    Celula() : valor(0), estado(Estado::VAZIO) {}
    explicit Celula(int val) : valor(val), estado(Estado::OCUPADO) {}
};

/**
 * @brief Tabela Hash com endereçamento aberto (sondagem linear)
 */
class TabelaHashAberta {
private:
    std::vector<Celula> tabela;
    size_t tamanho;
    size_t numElementos;
    static constexpr double CONSTANTE_MULTIPLICACAO = 0.63274838;
    static constexpr double MAX_FATOR_CARGA = 0.75;
    
public:
    enum class TipoHash { DIVISAO, MULTIPLICACAO };
    
    explicit TabelaHashAberta(size_t tam);
    
    void inserir(int valor, TipoHash tipo);
    bool buscar(int valor, TipoHash tipo) const;
    std::optional<int> remover(int valor, TipoHash tipo);
    
    size_t calcularHashDivisao(int chave) const;
    size_t calcularHashMultiplicacao(int chave) const;
    double fatorCarga() const { return static_cast<double>(numElementos) / tamanho; }
    bool precisaRehash() const { return fatorCarga() > MAX_FATOR_CARGA; }
    size_t getNumElementos() const { return numElementos; }
    
private:
    size_t sondagemLinear(size_t indiceInicial, int valor) const;
};