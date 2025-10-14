#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <random>
#include <filesystem>
#include <iostream>

/**
 * @brief Classe responsável por gerenciar dados para testes de tabelas hash
 * 
 * Esta classe fornece funcionalidades para:
 * - Carregar dados de arquivos existentes na pasta data/
 * - Gerar dados aleatórios para testes adicionais
 * - Validar a integridade dos arquivos de dados
 * - Fornecer estatísticas sobre os datasets
 * 
 * O formato esperado dos arquivos é:
 * - Primeira linha: quantidade total de números
 * - Linhas subsequentes: um número inteiro por linha
 */
class GeradorDados {
private:
    std::mt19937 gerador;                           ///< Gerador de números aleatórios
    std::uniform_int_distribution<int> distribuicao; ///< Distribuição uniforme
    
    /**
     * @brief Valida se um arquivo existe e pode ser lido
     * @param nomeArquivo Caminho do arquivo a ser validado
     * @return true se o arquivo existe e é legível
     */
    bool arquivoExiste(const std::string& nomeArquivo) const {
        std::ifstream arquivo(nomeArquivo);
        return arquivo.good();
    }
    
    /**
     * @brief Remove espaços em branco de uma string
     * @param str String a ser processada
     * @return String sem espaços nas extremidades
     */
    std::string trim(const std::string& str) const {
        size_t primeiro = str.find_first_not_of(" \t\n\r");
        if (primeiro == std::string::npos) {
            return "";
        }
        size_t ultimo = str.find_last_not_of(" \t\n\r");
        return str.substr(primeiro, (ultimo - primeiro + 1));
    }

public:
    /**
     * @brief Construtor com parâmetros customizáveis
     * @param seed Semente para o gerador aleatório
     * @param minimo Valor mínimo para geração aleatória
     * @param maximo Valor máximo para geração aleatória
     */
    explicit GeradorDados(unsigned int seed = std::random_device{}(), 
                         int minimo = 1, 
                         int maximo = 1000000)
        : gerador(seed), distribuicao(minimo, maximo) {
        if (minimo >= maximo) {
            throw std::invalid_argument("Valor mínimo deve ser menor que o máximo");
        }
    }
    
    /**
     * @brief Carrega números de um arquivo de texto
     * 
     * Formato esperado:
     * - Primeira linha: quantidade de números (usado para validação)
     * - Linhas seguintes: um número por linha
     * 
     * @param nomeArquivo Caminho do arquivo de entrada
     * @return Vetor com os números carregados
     * @throws std::runtime_error se o arquivo não existir ou formato inválido
     */
    std::vector<int> carregarDeArquivo(const std::string& nomeArquivo);
    
    /**
     * @brief Gera números aleatórios únicos
     * @param quantidade Número de elementos a serem gerados
     * @return Vetor com números únicos
     * @throws std::invalid_argument se quantidade for zero
     */
    std::vector<int> gerarNumerosAleatorios(size_t quantidade);
    
    /**
     * @brief Gera números aleatórios permitindo repetições
     * @param quantidade Número de elementos a serem gerados
     * @return Vetor com números (podem haver repetições)
     * @throws std::invalid_argument se quantidade for zero
     */
    std::vector<int> gerarNumerosAleatoriosComRepeticao(size_t quantidade);
    
    /**
     * @brief Salva números em um arquivo
     * @param numeros Vetor com os números a serem salvos
     * @param nomeArquivo Caminho do arquivo de saída
     * @return true se salvou com sucesso, false caso contrário
     */
    bool salvarEmArquivo(const std::vector<int>& numeros, const std::string& nomeArquivo);
    
    /**
     * @brief Valida a estrutura de um arquivo de dados
     * @param nomeArquivo Arquivo a ser validado
     * @return true se o arquivo está no formato correto
     */
    bool validarArquivo(const std::string& nomeArquivo) const;
    
    /**
     * @brief Lista todos os arquivos disponíveis na pasta data/
     * @return Vetor com os caminhos dos arquivos encontrados
     */
    std::vector<std::string> listarArquivosDisponiveis() const {
        std::vector<std::string> arquivos;
        
        if (!std::filesystem::exists("data/")) {
            return arquivos;
        }
        
        for (const auto& entrada : std::filesystem::directory_iterator("data/")) {
            if (entrada.is_regular_file() && entrada.path().extension() == ".txt") {
                arquivos.push_back(entrada.path().string());
            }
        }
        
        std::sort(arquivos.begin(), arquivos.end());
        return arquivos;
    }
    
    /**
     * @brief Obtém estatísticas básicas de um arquivo
     * @param nomeArquivo Caminho do arquivo
     */
    void exibirEstatisticas(const std::string& nomeArquivo) const;
    
    /**
     * @brief Gera todos os arquivos necessários conforme especificação do trabalho
     * @param diretorio Diretório onde salvar os arquivos (padrão: "data")
     */
    void gerarArquivosTrabalho(const std::string& diretorio = "data");
    
    /**
     * @brief Redefine a semente do gerador
     * @param novaSeed Nova semente
     */
    void redefinirSeed(unsigned int novaSeed) {
        gerador.seed(novaSeed);
    }
    
    /**
     * @brief Redefine o intervalo de valores
     * @param minimo Novo valor mínimo
     * @param maximo Novo valor máximo
     */
    void redefinirIntervalo(int minimo, int maximo) {
        if (minimo >= maximo) {
            throw std::invalid_argument("Valor mínimo deve ser menor que o máximo");
        }
        distribuicao = std::uniform_int_distribution<int>(minimo, maximo);
    }
    
    /**
     * @brief Estrutura com informações sobre um dataset
     */
    struct InfoDataset {
        std::string nomeArquivo;    ///< Nome do arquivo
        size_t quantidade;          ///< Número de elementos
        int minimo;                 ///< Menor valor
        int maximo;                 ///< Maior valor
        double media;               ///< Média dos valores
        bool temDuplicatas;         ///< Se contém valores duplicados
        size_t numDuplicatas;       ///< Número de duplicatas encontradas
    };
    
    /**
     * @brief Analisa estatísticas completas de um arquivo
     * @param nomeArquivo Caminho do arquivo
     * @return Estrutura com as informações
     * @throws std::runtime_error se houver erro ao analisar
     */
    InfoDataset analisarDataset(const std::string& nomeArquivo);
    
    /**
     * @brief Gera um relatório de todos os arquivos disponíveis
     */
    void gerarRelatorioDatasets() const;
};

/**
 * @brief Classe utilitaria para benchmarking de geração de dados
 */
class BenchmarkGeradorDados {
public:
    /**
     * @brief Testa diferentes métodos de geração
     * @param quantidades Tamanhos a serem testados
     */
    static void testarDesempenho(const std::vector<size_t>& quantidades);
};