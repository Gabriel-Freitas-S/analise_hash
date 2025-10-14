#ifndef GERADOR_DADOS_HPP
#define GERADOR_DADOS_HPP

#include <vector>
#include <string>
#include <random>
#include <stdexcept>
#include <fstream>

/**
 * @file GeradorDados.hpp
 * @brief Classe para geração e manipulação de conjuntos de dados numéricos
 * 
 * Esta classe fornece funcionalidades para gerar números aleatórios de alta qualidade
 * usando o Mersenne Twister, além de operações de leitura e escrita em arquivos.
 */

/**
 * @class GeradorDados
 * @brief Gerador de dados numéricos aleatórios com suporte a I/O de arquivos
 * 
 * Utiliza std::mt19937 (Mersenne Twister) para geração de números pseudo-aleatórios
 * de alta qualidade, com suporte a diferentes distribuições e persistência em arquivos.
 */
class GeradorDados {
public:
    /**
     * @brief Construtor com seed configurável
     * @param seed Semente para o gerador aleatório. Se não fornecida, usa std::random_device
     * @throw std::runtime_error Se não for possível inicializar o gerador
     */
    explicit GeradorDados(unsigned int seed = 0);
    
    /**
     * @brief Destrutor padrão
     */
    ~GeradorDados() = default;
    
    // Prevenir cópia (classe gerencia estado interno)
    GeradorDados(const GeradorDados&) = delete;
    GeradorDados& operator=(const GeradorDados&) = delete;
    
    // Permitir movimentação
    GeradorDados(GeradorDados&&) noexcept = default;
    GeradorDados& operator=(GeradorDados&&) noexcept = default;
    
    /**
     * @brief Gera vetor de números inteiros aleatórios com distribuição uniforme
     * @param quantidade Número de elementos a gerar
     * @param minimo Valor mínimo do intervalo (inclusivo)
     * @param maximo Valor máximo do intervalo (inclusivo)
     * @return std::vector<int> Vetor com números aleatórios gerados
     * @throw std::invalid_argument Se quantidade for 0 ou minimo > maximo
     */
    std::vector<int> gerarNumerosAleatorios(size_t quantidade, 
                                           int minimo = 1, 
                                           int maximo = 1000000);
    
    /**
     * @brief Gera vetor de números inteiros únicos (sem repetição)
     * @param quantidade Número de elementos únicos a gerar
     * @param minimo Valor mínimo do intervalo (inclusivo)
     * @param maximo Valor máximo do intervalo (inclusivo)
     * @return std::vector<int> Vetor com números únicos aleatórios
     * @throw std::invalid_argument Se quantidade > (maximo - minimo + 1)
     */
    std::vector<int> gerarNumerosUnicos(size_t quantidade,
                                       int minimo = 1,
                                       int maximo = 1000000);
    
    /**
     * @brief Salva vetor de números em arquivo de texto
     * @param numeros Vetor de números a salvar
     * @param nomeArquivo Caminho completo do arquivo de saída
     * @param umPorLinha Se true, cada número em uma linha; se false, separados por espaço
     * @return bool true se salvou com sucesso, false caso contrário
     * @throw std::runtime_error Se não for possível criar/escrever no arquivo
     */
    bool salvarEmArquivo(const std::vector<int>& numeros, 
                        const std::string& nomeArquivo,
                        bool umPorLinha = true);
    
    /**
     * @brief Carrega números de arquivo de texto
     * @param nomeArquivo Caminho completo do arquivo de entrada
     * @return std::vector<int> Vetor com números carregados
     * @throw std::runtime_error Se não for possível abrir/ler o arquivo
     * @throw std::invalid_argument Se o arquivo contiver dados inválidos
     */
    std::vector<int> carregarDeArquivo(const std::string& nomeArquivo);
    
    /**
     * @brief Gera conjunto completo de arquivos para teste
     * @param diretorioSaida Diretório onde os arquivos serão salvos
     * @param tamanhos Vetor com tamanhos dos conjuntos a gerar
     * @param minimo Valor mínimo do intervalo
     * @param maximo Valor máximo do intervalo
     * @return size_t Número de arquivos gerados com sucesso
     * @throw std::runtime_error Se não for possível criar os arquivos
     */
    size_t gerarArquivosTeste(const std::string& diretorioSaida,
                             const std::vector<size_t>& tamanhos,
                             int minimo = 1,
                             int maximo = 1000000);
    
    /**
     * @brief Define nova seed para o gerador
     * @param seed Nova semente
     */
    void setSeed(unsigned int seed);
    
    /**
     * @brief Obtém a seed atual do gerador
     * @return unsigned int Seed atual
     */
    unsigned int getSeed() const { return seedAtual; }
    
    /**
     * @brief Verifica se há entropia disponível via random_device
     * @return bool true se há entropia de hardware disponível
     */
    static bool temEntropiaHardware();

private:
    std::mt19937 gerador;                    ///< Gerador Mersenne Twister
    unsigned int seedAtual;                  ///< Seed atual do gerador
    
    /**
     * @brief Valida parâmetros de geração
     * @throw std::invalid_argument Se parâmetros forem inválidos
     */
    void validarParametros(size_t quantidade, int minimo, int maximo) const;
    
    /**
     * @brief Inicializa o gerador com entropia apropriada
     */
    void inicializarGerador(unsigned int seed);
};

#endif // GERADOR_DADOS_HPP