#pragma once

#include <vector>
#include <string>
#include <random>
#include <fstream>
#include <stdexcept>

/**
 * @brief Classe responsável por gerar e gerenciar conjuntos de dados para testes
 * 
 * Esta classe fornece funcionalidades para:
 * - Gerar números aleatórios com distribuição uniforme
 * - Salvar dados em arquivos de texto no formato especificado
 * - Carregar dados de arquivos existentes
 * - Validar e garantir a integridade dos dados
 */
class GeradorDados {
private:
    std::mt19937 gerador;                    ///< Gerador Mersenne Twister para números aleatórios
    std::uniform_int_distribution<int> distribuicao; ///< Distribuição uniforme
    
    /**
     * @brief Valida se um arquivo existe e pode ser lido
     * @param nomeArquivo Caminho do arquivo a ser validado
     * @return true se o arquivo existe e é legível, false caso contrário
     */
    bool arquivoExiste(const std::string& nomeArquivo) const;
    
    /**
     * @brief Remove espaços em branco de uma string
     * @param str String a ser processada
     * @return String sem espaços nas extremidades
     */
    std::string trim(const std::string& str) const;

public:
    /**
     * @brief Construtor padrão com seed configurável
     * @param seed Semente para o gerador aleatório (padrão: dispositivo aleatório)
     * @param minimo Valor mínimo para geração (padrão: 1)
     * @param maximo Valor máximo para geração (padrão: 1.000.000)
     */
    explicit GeradorDados(unsigned int seed = std::random_device{}(),
                          int minimo = 1,
                          int maximo = 1000000);
    
    /**
     * @brief Gera um vetor de números aleatórios únicos
     * @param quantidade Número de elementos a gerar
     * @return Vetor com números aleatórios
     * @throws std::invalid_argument se quantidade for inválida
     */
    std::vector<int> gerarNumerosAleatorios(size_t quantidade);
    
    /**
     * @brief Gera números aleatórios com possibilidade de repetição
     * @param quantidade Número de elementos a gerar
     * @return Vetor com números aleatórios
     */
    std::vector<int> gerarNumerosAleatoriosComRepeticao(size_t quantidade);
    
    /**
     * @brief Salva um vetor de números em arquivo de texto
     * 
     * Formato do arquivo:
     * - Primeira linha: quantidade de números
     * - Linhas seguintes: um número por linha
     * 
     * @param numeros Vetor de números a salvar
     * @param nomeArquivo Caminho do arquivo de saída
     * @return true se salvo com sucesso, false caso contrário
     * @throws std::runtime_error se não conseguir criar o arquivo
     */
    bool salvarEmArquivo(const std::vector<int>& numeros, 
                         const std::string& nomeArquivo);
    
    /**
     * @brief Carrega números de um arquivo de texto
     * 
     * Formato esperado:
     * - Primeira linha: quantidade de números (validado)
     * - Linhas seguintes: um número por linha
     * 
     * @param nomeArquivo Caminho do arquivo de entrada
     * @return Vetor com os números carregados
     * @throws std::runtime_error se o arquivo não existir ou formato inválido
     */
    std::vector<int> carregarDeArquivo(const std::string& nomeArquivo);
    
    /**
     * @brief Gera todos os arquivos de dados necessários para o trabalho
     * 
     * Cria os arquivos:
     * - numeros_100.txt
     * - numeros_500.txt
     * - numeros_1000.txt
     * - numeros_5000.txt
     * - numeros_10000.txt
     * - numeros_50000.txt
     * - busca_1000.txt
     * 
     * @param diretorio Diretório onde os arquivos serão salvos (padrão: "data/")
     * @throws std::runtime_error se não conseguir criar os arquivos
     */
    void gerarArquivosTrabalho(const std::string& diretorio = "data/");
    
    /**
     * @brief Redefine a seed do gerador aleatório
     * @param novaSeed Nova seed a ser utilizada
     */
    void redefinirSeed(unsigned int novaSeed);
    
    /**
     * @brief Redefine o intervalo de geração de números
     * @param minimo Novo valor mínimo
     * @param maximo Novo valor máximo
     * @throws std::invalid_argument se minimo >= maximo
     */
    void redefinirIntervalo(int minimo, int maximo);
    
    /**
     * @brief Valida a estrutura de um arquivo de dados
     * @param nomeArquivo Arquivo a ser validado
     * @return true se o arquivo está no formato correto
     */
    bool validarArquivo(const std::string& nomeArquivo) const;
};