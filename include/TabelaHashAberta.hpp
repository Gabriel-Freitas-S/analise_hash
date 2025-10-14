#pragma once

#include <vector>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <cmath>

/**
 * @brief Célula da tabela hash com endereçamento aberto
 * 
 * Cada célula pode estar em um dos três estados:
 * - VAZIO: Posição nunca foi ocupada
 * - OCUPADO: Posição contém um valor válido
 * - REMOVIDO: Posição já foi ocupada mas o valor foi removido
 */
struct Celula {
    /**
     * @brief Estados possíveis de uma célula
     */
    enum class Estado { 
        VAZIO,      ///< Célula nunca foi ocupada
        OCUPADO,    ///< Célula contém um valor válido
        REMOVIDO    ///< Célula foi ocupada e depois removida (lazy deletion)
    };
    
    int valor;      ///< Valor armazenado na célula
    Estado estado;  ///< Estado atual da célula
    
    /**
     * @brief Construtor padrão - cria célula vazia
     */
    Celula() : valor(0), estado(Estado::VAZIO) {}
    
    /**
     * @brief Construtor com valor - cria célula ocupada
     * @param val Valor a ser armazenado
     */
    explicit Celula(int val) : valor(val), estado(Estado::OCUPADO) {}
    
    /**
     * @brief Marca a célula como removida
     */
    void marcarRemovido() {
        estado = Estado::REMOVIDO;
        valor = 0; // Limpar valor por segurança
    }
    
    /**
     * @brief Verifica se a célula está disponível para inserção
     * @return true se a célula está vazia ou foi removida
     */
    bool disponivelParaInsercao() const {
        return estado == Estado::VAZIO || estado == Estado::REMOVIDO;
    }
};

/**
 * @brief Tabela Hash com endereçamento aberto usando sondagem linear
 * 
 * Esta implementação utiliza endereçamento aberto para resolver colisões,
 * onde elementos que colidem são colocados na próxima posição disponível
 * usando sondagem linear.
 * 
 * Suporta duas funções de hash:
 * - Método da Divisão: h(k) = k mod p
 * - Método da Multiplicação: h(k) = floor((k * c mod 1) * p)
 * 
 * Características:
 * - Lazy deletion (células removidas são marcadas, não apagadas)
 * - Controle automático de fator de carga
 * - Sondagem linear para resolução de colisões
 */
class TabelaHashAberta {
private:
    std::vector<Celula> tabela;     ///< Tabela de células
    size_t tamanho;                 ///< Tamanho da tabela
    size_t numElementos;            ///< Número de elementos ativos
    size_t numRemovidos;            ///< Número de elementos removidos
    
    /// Constante para o método da multiplicação (Fração áurea)
    static constexpr double CONSTANTE_MULTIPLICACAO = 0.6180339887;
    
    /// Fator de carga máximo recomendado para manter performance
    static constexpr double MAX_FATOR_CARGA = 0.7;
    
    /// Limite para rehash considerando elementos removidos
    static constexpr double LIMITE_REHASH_REMOVIDOS = 0.5;
    
    /**
     * @brief Realiza sondagem linear para encontrar uma posição
     * @param indiceInicial Índice inicial calculado pela função hash
     * @param valor Valor sendo procurado (para verificação de existência)
     * @param paraInsercao Se true, procura posição para inserção; se false, para busca
     * @return Índice encontrado ou tamanho da tabela se não encontrou
     */
    size_t sondagemLinear(size_t indiceInicial, int valor, bool paraInsercao = false) const;
    
public:
    /**
     * @brief Enumeração dos tipos de função hash suportados
     */
    enum class TipoHash { 
        DIVISAO,        ///< Método da divisão
        MULTIPLICACAO   ///< Método da multiplicação
    };
    
    /**
     * @brief Construtor da tabela hash aberta
     * @param tam Tamanho da tabela
     * @throws std::invalid_argument se o tamanho for zero
     */
    explicit TabelaHashAberta(size_t tam) 
        : tamanho(tam), numElementos(0), numRemovidos(0) {
        if (tam == 0) {
            throw std::invalid_argument("Tamanho da tabela deve ser maior que zero");
        }
        tabela.resize(tamanho);
    }
    
    /**
     * @brief Destructor padrão
     */
    ~TabelaHashAberta() = default;
    
    // Permitir cópia e movimentação
    TabelaHashAberta(const TabelaHashAberta&) = default;
    TabelaHashAberta& operator=(const TabelaHashAberta&) = default;
    TabelaHashAberta(TabelaHashAberta&&) noexcept = default;
    TabelaHashAberta& operator=(TabelaHashAberta&&) noexcept = default;
    
    /**
     * @brief Insere um valor na tabela hash
     * @param valor Valor a ser inserido
     * @param tipo Tipo da função hash a ser utilizada
     * @throws std::runtime_error se a tabela estiver cheia ou fator de carga alto
     */
    void inserir(int valor, TipoHash tipo);
    
    /**
     * @brief Busca um valor na tabela hash
     * @param valor Valor a ser buscado
     * @param tipo Tipo da função hash utilizada
     * @return true se o valor foi encontrado, false caso contrário
     */
    bool buscar(int valor, TipoHash tipo) const;
    
    /**
     * @brief Remove um valor da tabela hash (lazy deletion)
     * @param valor Valor a ser removido
     * @param tipo Tipo da função hash utilizada
     * @return Valor removido se encontrado, std::nullopt caso contrário
     */
    std::optional<int> remover(int valor, TipoHash tipo);
    
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
     * @brief Calcula o fator de carga atual (apenas elementos ativos)
     * @return Fator de carga
     */
    double fatorCarga() const { 
        return static_cast<double>(numElementos) / tamanho; 
    }
    
    /**
     * @brief Calcula o fator de ocupação total (incluindo removidos)
     * @return Fator de ocupação
     */
    double fatorOcupacao() const {
        return static_cast<double>(numElementos + numRemovidos) / tamanho;
    }
    
    /**
     * @brief Verifica se a tabela precisa de rehash
     * @return true se o fator de carga ou ocupação está muito alto
     */
    bool precisaRehash() const { 
        return fatorCarga() > MAX_FATOR_CARGA || 
               fatorOcupacao() > LIMITE_REHASH_REMOVIDOS; 
    }
    
    /**
     * @brief Obtém o número de elementos ativos
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
     * @brief Obtém o número de elementos removidos
     * @return Número de remoções
     */
    size_t getNumRemovidos() const { 
        return numRemovidos; 
    }
    
    /**
     * @brief Verifica se a tabela está vazia
     * @return true se não houver elementos ativos
     */
    bool vazia() const { 
        return numElementos == 0; 
    }
    
    /**
     * @brief Limpa todos os elementos da tabela
     */
    void limpar() {
        for (auto& celula : tabela) {
            celula = Celula();
        }
        numElementos = 0;
        numRemovidos = 0;
    }
    
    /**
     * @brief Estatísticas sobre sondagem e distribuição
     */
    struct EstatisticasSondagem {
        size_t totalSondagens;      ///< Total de sondagens realizadas em operações
        double sondagemMedia;       ///< Número médio de sondagens por operação
        size_t maxSondagens;        ///< Máximo de sondagens em uma operação
        size_t clustersDetectados;  ///< Número de clusters contíguos
        size_t maiorCluster;        ///< Tamanho do maior cluster
    };
    
    /**
     * @brief Analisa estatísticas de clustering e sondagem
     * @return Estrutura com as estatísticas
     */
    EstatisticasSondagem analisarSondagem() const;
};

/**
 * @brief Overload do operador de saída para TipoHash
 * @param os Stream de saída
 * @param tipo Tipo de hash a ser impresso
 * @return Referência para o stream
 */
std::ostream& operator<<(std::ostream& os, TabelaHashAberta::TipoHash tipo);

/**
 * @brief Overload do operador de saída para Estado da Célula
 * @param os Stream de saída
 * @param estado Estado a ser impresso
 * @return Referência para o stream
 */
std::ostream& operator<<(std::ostream& os, Celula::Estado estado);