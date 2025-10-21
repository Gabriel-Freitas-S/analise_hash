/**
 * @file CarregadorDados.hpp
 * @brief Definição da classe CarregadorDados para gerenciamento de datasets
 * 
 * Este arquivo define a classe CarregadorDados, responsável pelo carregamento
 * de conjuntos de dados de arquivos para testes de tabelas hash. Implementa
 * as funcionalidades essenciais requeridas pelo Trabalho 2: carregamento
 * de datasets e geração de dados para busca.
 * 
 * @author Gabriel Freitas Souza
 * @author Roberli Schuina Silva
 * @date 2024-10-18
 * @version 1.1
 * 
 * Características principais:
 * - Carregamento eficiente de datasets de arquivos de texto
 * - Geração de números aleatórios para testes de busca
 * - Validação da integridade dos arquivos
 * - Coleta de estatísticas sobre os datasets
 */

#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <random>
#include <filesystem>
#include <set>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <numeric>

/**
 * @brief Classe CarregadorDados - Gerenciador de datasets para testes
 * 
 * Esta classe centraliza as operações relacionadas ao gerenciamento
 * de dados para os testes de tabelas hash, conforme especificado no Trabalho 2.
 * 
 * Principais responsabilidades:
 * - Carregar datasets da pasta data/
 * - Gerar dados aleatórios para testes de busca (1000 números entre 1 e 1.000.000)
 * - Validar integridade de arquivos
 * - Fornecer estatísticas básicas sobre os dados
 */
class CarregadorDados {
private:
    std::mt19937 gerador;                           ///< Gerador de números aleatórios
    std::uniform_int_distribution<int> distribuicao; ///< Distribuição uniforme
    
    /**
     * @brief Verifica se um arquivo existe no sistema
     * @param nomeArquivo Caminho completo para o arquivo
     * @return true se o arquivo existe e é acessível
     */
    bool arquivoExiste(const std::string& nomeArquivo) const {
        return std::filesystem::exists(nomeArquivo) && 
               std::filesystem::is_regular_file(nomeArquivo);
    }
    
    /**
     * @brief Remove espaços em branco do início e fim de uma string
     * @param str String a ser processada
     * @return String sem espaços nas extremidades
     */
    std::string trim(const std::string& str) const {
        size_t inicio = str.find_first_not_of(" \t\n\r");
        if (inicio == std::string::npos) return "";
        
        size_t fim = str.find_last_not_of(" \t\n\r");
        return str.substr(inicio, fim - inicio + 1);
    }

public:
    /**
     * @brief Construtor do CarregadorDados
     * @param seed Semente para o gerador de números aleatórios
     * @param minimo Valor mínimo para geração aleatória (padrão: 1)
     * @param maximo Valor máximo para geração aleatória (padrão: 1.000.000)
     * 
     * Inicializa o gerador conforme especificação do Trabalho 2:
     * números aleatórios entre 1 e 1.000.000.
     */
    explicit CarregadorDados(unsigned int seed = std::random_device{}(), 
                            int minimo = 1, 
                            int maximo = 1000000) 
        : gerador(seed), distribuicao(minimo, maximo) {
        if (minimo >= maximo) {
            throw std::invalid_argument("Valor mínimo deve ser menor que o máximo");
        }
    }
    
    /**
     * @brief Carrega números de um arquivo de texto
     * @param nomeArquivo Caminho para o arquivo de dados
     * @return Vetor com os números carregados
     * @throws std::runtime_error se arquivo não existir ou estiver corrompido
     * 
     * Formato esperado conforme Trabalho 2:
     * - Primeira linha: quantidade de números
     * - Linhas seguintes: um número inteiro por linha
     * 
     * @complexity O(n) onde n é o número de elementos no arquivo
     */
    std::vector<int> carregarDeArquivo(const std::string& nomeArquivo);
    
    /**
     * @brief Gera vetor de números aleatórios únicos
     * @param quantidade Número de elementos a gerar
     * @return Vetor com números aleatórios sem duplicatas
     * 
     * Para quantidades grandes (>10.000), delega para geração com repetição
     * por questões de performance.
     * 
     * @complexity O(n) amortizada
     */
    std::vector<int> gerarNumerosAleatorios(size_t quantidade);
    
    /**
     * @brief Gera vetor de números aleatórios permitindo duplicatas
     * @param quantidade Número de elementos a gerar
     * @return Vetor com números aleatórios (pode conter duplicatas)
     * 
     * Método otimizado usado para gerar os 1000 números aleatórios
     * para busca conforme especificado no Trabalho 2.
     * 
     * @complexity O(n) linear
     */
    std::vector<int> gerarNumerosAleatoriosComRepeticao(size_t quantidade);
    
    /**
     * @brief Salva vetor de números em arquivo de texto
     * @param numeros Vetor de números a salvar
     * @param nomeArquivo Caminho do arquivo de destino
     * @return true se salvou com sucesso
     * 
     * Salva no formato padronizado:
     * - Primeira linha: quantidade
     * - Linhas seguintes: um número por linha
     * 
     * @complexity O(n)
     */
    bool salvarEmArquivo(const std::vector<int>& numeros, const std::string& nomeArquivo);
    
    /**
     * @brief Valida a integridade de um arquivo de dados
     * @param nomeArquivo Caminho do arquivo a validar
     * @return true se o arquivo é válido
     * 
     * Verifica se o arquivo está no formato correto e é consistente.
     * 
     * @complexity O(n)
     */
    bool validarArquivo(const std::string& nomeArquivo) const;
    
    /**
     * @brief Lista todos os arquivos disponíveis na pasta data/
     * @return Vetor com nomes dos arquivos encontrados
     * 
     * Busca por arquivos .txt na pasta data/ conforme estrutura do projeto.
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
     * @brief Exibe estatísticas detalhadas sobre um dataset
     * @param nomeArquivo Arquivo a ser analisado
     * 
     * Mostra informações como quantidade, intervalo, média e duplicatas.
     * 
     * @complexity O(n)
     */
    void exibirEstatisticas(const std::string& nomeArquivo) const;
    
    /**
     * @brief Estrutura com informações sobre um dataset
     * 
     * Contém as estatísticas básicas calculadas sobre um dataset.
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
     * @throws std::runtime_error se não conseguir analisar
     * 
     * Calcula todas as estatísticas básicas do dataset.
     * 
     * @complexity O(n)
     */
    InfoDataset analisarDataset(const std::string& nomeArquivo);
};