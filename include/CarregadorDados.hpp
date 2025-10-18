/**
 * @file CarregadorDados.hpp
 * @brief Definição da classe CarregadorDados para gerenciamento de datasets
 * 
 * Este arquivo define a classe CarregadorDados, responsável pelo carregamento
 * de conjuntos de dados de arquivos para testes de tabelas hash. Além do
 * carregamento principal, oferece funcionalidades auxiliares como geração
 * de dados aleatórios, validação de arquivos e coleta de estatísticas.
 * 
 * @author Gabriel Freitas Souza
 * @author Roberli Schuina Silva
 * @date 2024-10-18
 * @version 1.0
 * 
 * Características principais:
 * - Carregamento eficiente de datasets de arquivos de texto
 * - Geração de números aleatórios para testes complementares
 * - Validação da integridade dos arquivos
 * - Coleta de estatísticas sobre os datasets
 * - Suporte a diferentes formatos de arquivo
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
 * Esta classe centraliza todas as operações relacionadas ao gerenciamento
 * de dados para os testes de tabelas hash. Sua função principal é carregar
 * dados de arquivos existentes, mas também oferece funcionalidades para:
 * 
 * Principais responsabilidades:
 * - Carregar datasets da pasta data/
 * - Gerar dados aleatórios para testes
 * - Validar integridade de arquivos
 * - Fornecer estatísticas sobre os dados
 * - Salvar datasets gerados
 * 
 * O nome "Carregador" reflete sua função principal, que é carregar
 * dados já existentes, diferentemente de um "gerador" que criaria
 * dados do zero.
 */
class CarregadorDados {
private:
    std::mt19937 gerador;                           ///< Gerador de números aleatórios
    std::uniform_int_distribution<int> distribuicao; ///< Distribuição uniforme
    
    /**
     * @brief Verifica se um arquivo existe no sistema
     * @param nomeArquivo Caminho completo para o arquivo
     * @return true se o arquivo existe e é acessível
     * 
     * Utiliza std::filesystem para verificar existência de forma robusta.
     */
    bool arquivoExiste(const std::string& nomeArquivo) const {
        return std::filesystem::exists(nomeArquivo) && 
               std::filesystem::is_regular_file(nomeArquivo);
    }
    
    /**
     * @brief Remove espaços em branco do início e fim de uma string
     * @param str String a ser processada
     * @return String sem espaços nas extremidades
     * 
     * Função utilitária para limpeza de dados lidos de arquivos.
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
     * @param minimo Valor mínimo para geração aleatória
     * @param maximo Valor máximo para geração aleatória
     * 
     * Inicializa o gerador de números aleatórios com a semente especificada.
     * Se nenhuma semente for fornecida, usa std::random_device para
     * obter uma semente não determinística.
     * 
     * Parâmetros padrão cobrem uma ampla faixa de valores inteiros.
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
     * Formato esperado do arquivo:
     * - Primeira linha: quantidade de números (opcional para validação)
     * - Linhas seguintes: um número inteiro por linha
     * - Linhas vazias e espaços são ignorados
     * 
     * A função é robusta e tenta recuperar de pequenos problemas
     * de formato, como espaços extras.
     */
    std::vector<int> carregarDeArquivo(const std::string& nomeArquivo);
    
    /**
     * @brief Gera vetor de números aleatórios únicos
     * @param quantidade Número de elementos a gerar
     * @return Vetor com números aleatórios sem duplicatas
     * 
     * Gera um conjunto de números aleatórios garantindo que não
     * há duplicatas. Utiliza std::set internamente para garantir
     * unicidade, depois converte para vetor.
     */
    std::vector<int> gerarNumerosAleatorios(size_t quantidade);
    
    /**
     * @brief Gera vetor de números aleatórios permitindo duplicatas
     * @param quantidade Número de elementos a gerar
     * @return Vetor com números aleatórios (pode conter duplicatas)
     * 
     * Versão mais rápida que permite duplicatas. Útil para testes
     * onde duplicatas são aceitáveis ou desejadas.
     */
    std::vector<int> gerarNumerosAleatoriosComRepeticao(size_t quantidade);
    
    /**
     * @brief Salva vetor de números em arquivo de texto
     * @param numeros Vetor de números a salvar
     * @param nomeArquivo Caminho do arquivo de destino
     * @return true se salvou com sucesso, false caso contrário
     */
    bool salvarEmArquivo(const std::vector<int>& numeros, const std::string& nomeArquivo);
    
    /**
     * @brief Valida a integridade de um arquivo de dados
     * @param nomeArquivo Caminho do arquivo a validar
     * @return true se o arquivo é válido, false caso contrário
     */
    bool validarArquivo(const std::string& nomeArquivo) const;
    
    /**
     * @brief Lista todos os arquivos disponíveis na pasta data/
     * @return Vetor com nomes dos arquivos encontrados
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
     */
    void exibirEstatisticas(const std::string& nomeArquivo) const;
    
    /**
     * @brief Gera todos os arquivos necessários para o trabalho
     * @param diretorio Diretório de destino
     */
    void gerarArquivosTrabalho(const std::string& diretorio = "data");
    
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
     */
    InfoDataset analisarDataset(const std::string& nomeArquivo);
};

/**
 * @brief Classe utilitária para benchmarking de carregamento de dados
 */
class BenchmarkCarregadorDados {
public:
    /**
     * @brief Testa diferentes métodos de geração e carregamento
     * @param quantidades Tamanhos a serem testados
     */
    static void testarDesempenho(const std::vector<size_t>& quantidades);
};