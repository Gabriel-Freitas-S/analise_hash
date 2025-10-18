/**
 * @file TabelaAberta.hpp
 * @brief Definição da classe TabelaAberta para tabelas hash com endereçamento aberto
 * 
 * Este arquivo contém a definição completa da classe TabelaAberta, que implementa
 * uma tabela hash utilizando endereçamento aberto (open addressing) com sondagem
 * linear para resolver colisões. Esta abordagem armazena todos os elementos
 * diretamente no array da tabela, sem usar estruturas auxiliares.
 * 
 * @author Gabriel Freitas Souza
 * @author Roberli Schuina Silva
 * @date 2024-10-18
 * @version 1.0
 * 
 * Características principais:
 * - Sondagem linear para resolução de colisões
 * - Lazy deletion (marcação de elementos removidos)
 * - Controle automático de fator de carga
 * - Maior eficiência de memória
 * - Melhor localidade de cache
 * - Suporte a análise de clustering
 */

#pragma once

#include <vector>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <cmath>

/**
 * @brief Estrutura que representa uma célula da tabela hash
 * 
 * Cada célula pode estar em um dos três estados possíveis:
 * - VAZIO: Nunca foi ocupada
 * - OCUPADO: Contém um valor válido
 * - REMOVIDO: Já foi ocupada mas o valor foi removido (lazy deletion)
 * 
 * O uso de lazy deletion permite que a sondagem linear continue funcionando
 * corretamente mesmo após remoções, evitando a necessidade de reorganizar
 * a tabela após cada remoção.
 */
struct Celula {
    /**
     * @brief Estados possíveis de uma célula
     * 
     * O estado determina como a célula é tratada durante operações:
     * - VAZIO: Para a busca, permite inserção
     * - OCUPADO: Contém dado válido, continua busca se não for o procurado
     * - REMOVIDO: Continua busca, permite inserção
     */
    enum class Estado { 
        VAZIO,      ///< Célula nunca foi ocupada - para busca e permite inserção
        OCUPADO,    ///< Célula contém um valor válido
        REMOVIDO    ///< Célula foi ocupada mas valor foi removido (lazy deletion)
    };
    
    int valor;      ///< Valor armazenado na célula
    Estado estado;  ///< Estado atual da célula
    
    /**
     * @brief Construtor padrão - cria célula vazia
     * 
     * Inicializa a célula no estado VAZIO com valor zerado.
     */
    Celula() : valor(0), estado(Estado::VAZIO) {}
    
    /**
     * @brief Construtor com valor - cria célula ocupada
     * @param val Valor a ser armazenado na célula
     * 
     * Inicializa a célula no estado OCUPADO com o valor especificado.
     */
    explicit Celula(int val) : valor(val), estado(Estado::OCUPADO) {}
    
    /**
     * @brief Marca a célula como removida (lazy deletion)
     * 
     * Implementa lazy deletion mudando apenas o estado da célula.
     * O valor é zerado por segurança, mas o importante é o estado.
     */
    void marcarRemovido() {
        estado = Estado::REMOVIDO;
        valor = 0; // Limpar valor por segurança
    }
    
    /**
     * @brief Verifica se a célula está disponível para inserção
     * @return true se a célula está vazia ou foi removida
     * 
     * Uma célula está disponível para inserção se estiver VAZIA
     * ou REMOVIDA. Células OCUPADAS não estão disponíveis.
     */
    bool disponivelParaInsercao() const {
        return estado == Estado::VAZIO || estado == Estado::REMOVIDO;
    }
};

/**
 * @brief Classe TabelaAberta - Implementação de tabela hash com endereçamento aberto
 * 
 * Esta classe implementa uma tabela hash utilizando endereçamento aberto
 * (open addressing) com sondagem linear para resolução de colisões.
 * Todos os elementos são armazenados diretamente no array da tabela.
 * 
 * Vantagens do endereçamento aberto:
 * - Maior eficiência de memória (sem ponteiros)
 * - Melhor localidade de cache
 * - Menos alocações dinâmicas
 * - Performance excelente com baixo fator de carga
 * 
 * Desvantagens:
 * - Performance degrada rapidamente com alto fator de carga
 * - Requer controle cuidadoso do fator de carga
 * - Clustering primário pode ser problemático
 * - Remoção requer lazy deletion
 */
class TabelaAberta {
private:
    std::vector<Celula> tabela;     ///< Array de células da tabela hash
    size_t tamanho;                 ///< Tamanho total da tabela
    size_t numElementos;            ///< Número de elementos ativos (não removidos)
    size_t numRemovidos;            ///< Número de elementos removidos (lazy deletion)
    
    /// Constante para o método da multiplicação (proporção áurea)
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
     * 
     * A sondagem linear incrementa o índice sequencialmente até encontrar:
     * - Para inserção: célula VAZIA ou REMOVIDA
     * - Para busca: o valor procurado ou célula VAZIA (indica não existência)
     * 
     * @complexity O(1) média, O(n) no pior caso (clustering)
     */
    size_t sondagemLinear(size_t indiceInicial, int valor, bool paraInsercao = false) const;
    
public:
    /**
     * @brief Enumeração dos tipos de função hash suportados
     * 
     * Define os dois métodos de hashing implementados:
     * - DIVISAO: h(k) = k mod m (método da divisão)
     * - MULTIPLICACAO: h(k) = floor(m * ((k * A) mod 1)) (método da multiplicação)
     */
    enum class TipoHash { 
        DIVISAO,        ///< Método da divisão - simples e eficiente
        MULTIPLICACAO   ///< Método da multiplicação - melhor distribuição
    };
    
    /**
     * @brief Construtor da tabela hash aberta
     * @param tam Tamanho da tabela
     * @throws std::invalid_argument se o tamanho for zero
     * 
     * Inicializa a tabela com o tamanho especificado. Para endereçamento
     * aberto, o tamanho deve ser significativamente maior que o número
     * de elementos esperado para manter boa performance.
     */
    explicit TabelaAberta(size_t tam) 
        : tamanho(tam), numElementos(0), numRemovidos(0) {
        if (tam == 0) {
            throw std::invalid_argument("Tamanho da tabela deve ser maior que zero");
        }
        
        // Inicializa todas as células como vazias
        tabela.resize(tamanho);
    }
    
    /**
     * @brief Destrutor padrão
     * 
     * O destrutor padrão é suficiente pois usamos std::vector
     * que gerencia automaticamente a memória.
     */
    ~TabelaAberta() = default;
    
    // Permite cópia e movimentação (pode ser útil para testes)
    TabelaAberta(const TabelaAberta&) = default;
    TabelaAberta& operator=(const TabelaAberta&) = default;
    TabelaAberta(TabelaAberta&&) noexcept = default;
    TabelaAberta& operator=(TabelaAberta&&) noexcept = default;
    
    /**
     * @brief Insere um valor na tabela hash
     * @param valor Valor a ser inserido
     * @param tipo Tipo da função hash a ser utilizada
     * @throws std::runtime_error se a tabela estiver cheia ou fator de carga alto
     * 
     * Utiliza sondagem linear para encontrar uma posição disponível.
     * Não permite duplicatas.
     * 
     * @complexity O(1) média, O(n) no pior caso com clustering severo
     */
    void inserir(int valor, TipoHash tipo);
    
    /**
     * @brief Busca um valor na tabela hash
     * @param valor Valor a ser buscado
     * @param tipo Tipo da função hash utilizada
     * @return true se o valor foi encontrado, false caso contrário
     * 
     * Utiliza sondagem linear a partir da posição inicial calculada
     * pela função hash até encontrar o valor ou uma célula vazia.
     * 
     * @complexity O(1) média, O(n) no pior caso
     */
    bool buscar(int valor, TipoHash tipo) const;
    
    /**
     * @brief Remove um valor da tabela hash (lazy deletion)
     * @param valor Valor a ser removido
     * @param tipo Tipo da função hash utilizada
     * @return Valor removido se encontrado, std::nullopt caso contrário
     * 
     * Implementa lazy deletion: marca a célula como REMOVIDA
     * ao invés de realmente remover o elemento. Isso mantém
     * a integridade da sondagem linear.
     * 
     * @complexity O(1) média, O(n) no pior caso
     */
    std::optional<int> remover(int valor, TipoHash tipo);
    
    /**
     * @brief Calcula o índice usando o método da divisão
     * @param chave Chave a ser mapeada
     * @return Índice na tabela (0 <= índice < tamanho)
     * 
     * Implementa h(k) = k mod m, onde m é o tamanho da tabela.
     */
    size_t calcularHashDivisao(int chave) const {
        return static_cast<size_t>(std::abs(chave)) % tamanho;
    }
    
    /**
     * @brief Calcula o índice usando o método da multiplicação
     * @param chave Chave a ser mapeada
     * @return Índice na tabela (0 <= índice < tamanho)
     * 
     * Implementa h(k) = floor(m * ((k * A) mod 1)), onde:
     * - m é o tamanho da tabela
     * - A é a constante de multiplicação (proporção áurea)
     */
    size_t calcularHashMultiplicacao(int chave) const {
        double produto = std::abs(chave) * CONSTANTE_MULTIPLICACAO;
        double fracao = produto - std::floor(produto);
        return static_cast<size_t>(std::floor(fracao * tamanho));
    }
    
    /**
     * @brief Calcula o fator de carga atual (apenas elementos ativos)
     * @return Fator de carga (elementos ativos / tamanho da tabela)
     * 
     * O fator de carga considera apenas elementos não removidos.
     * Para endereçamento aberto, recomenda-se manter abaixo de 0.7.
     */
    double fatorCarga() const { 
        return static_cast<double>(numElementos) / tamanho; 
    }
    
    /**
     * @brief Calcula o fator de ocupação total (incluindo removidos)
     * @return Fator de ocupação total
     * 
     * Considera tanto elementos ativos quanto removidos.
     * Útil para decidir quando fazer rehash.
     */
    double fatorOcupacao() const {
        return static_cast<double>(numElementos + numRemovidos) / tamanho;
    }
    
    /**
     * @brief Verifica se a tabela precisa de rehash
     * @return true se o fator de carga ou ocupação está muito alto
     * 
     * Considera tanto o fator de carga dos elementos ativos
     * quanto o fator de ocupação total (incluindo removidos).
     */
    bool precisaRehash() const { 
        return fatorCarga() > MAX_FATOR_CARGA || 
               fatorOcupacao() > LIMITE_REHASH_REMOVIDOS; 
    }
    
    /**
     * @brief Obtém o número de elementos ativos
     * @return Número de elementos não removidos
     */
    size_t getNumElementos() const { 
        return numElementos; 
    }
    
    /**
     * @brief Obtém o tamanho da tabela
     * @return Número total de posições na tabela
     */
    size_t getTamanho() const { 
        return tamanho; 
    }
    
    /**
     * @brief Obtém o número de elementos removidos
     * @return Número de remoções (lazy deletion)
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
     * @brief Remove todos os elementos da tabela
     * 
     * Redefine todas as células para o estado VAZIO
     * e zera os contadores.
     */
    void limpar() {
        for (auto& celula : tabela) {
            celula = Celula(); // Reinicializa como vazia
        }
        numElementos = 0;
        numRemovidos = 0;
    }
    
    /**
     * @brief Estrutura para estatísticas de sondagem e clustering
     * 
     * Contém informações sobre o comportamento da sondagem linear
     * e a formação de clusters na tabela.
     */
    struct EstatisticasSondagem {
        size_t totalSondagens;      ///< Total de sondagens realizadas em operações
        double sondagemMedia;       ///< Número médio de sondagens por operação
        size_t maxSondagens;        ///< Máximo de sondagens em uma operação
        size_t clustersDetectados;  ///< Número de clusters contíguos detectados
        size_t maiorCluster;        ///< Tamanho do maior cluster encontrado
    };
    
    /**
     * @brief Analisa estatísticas de clustering e sondagem
     * @return Estrutura com as estatísticas coletadas
     * 
     * Percorre a tabela identificando clusters de células ocupadas
     * e simula operações para calcular estatísticas de sondagem.
     * 
     * @complexity O(n) onde n é o tamanho da tabela
     */
    EstatisticasSondagem analisarSondagem() const;
};

/**
 * @brief Operador de saída para TipoHash
 * @param os Stream de saída
 * @param tipo Tipo de hash a ser impresso
 * @return Referência para o stream
 * 
 * Permite impressão direta do tipo de hash usando cout << tipo.
 */
std::ostream& operator<<(std::ostream& os, TabelaAberta::TipoHash tipo);

/**
 * @brief Operador de saída para Estado da Célula
 * @param os Stream de saída
 * @param estado Estado a ser impresso
 * @return Referência para o stream
 * 
 * Permite impressão direta do estado da célula para debugging.
 */
std::ostream& operator<<(std::ostream& os, Celula::Estado estado);