/**
 * @file CarregadorDados.cpp
 * @brief Implementação da classe CarregadorDados
 * 
 * Este arquivo contém a implementação dos métodos essenciais da classe
 * CarregadorDados, focando nas funcionalidades requeridas pelo Trabalho 2:
 * carregamento de arquivos e geração de dados para busca.
 * 
 * @author Gabriel Freitas Souza
 * @author Roberli Schuina Silva
 * @date 2024-10-18
 * @version 1.1
 */

#include "CarregadorDados.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <set>
#include <unordered_set>
#include <numeric>
#include <chrono>
#include <iomanip>

/**
 * @brief Carrega dados de um arquivo de texto
 * 
 * Lê um arquivo no formato especificado pelo Trabalho 2:
 * - Primeira linha: quantidade de números
 * - Linhas seguintes: um número por linha
 * 
 * @param nomeArquivo Caminho para o arquivo de dados
 * @return Vetor com números carregados
 * @throws std::runtime_error se arquivo inacessível ou formato inválido
 * 
 * @complexity O(n) onde n é o número de linhas no arquivo
 */
std::vector<int> CarregadorDados::carregarDeArquivo(const std::string& nomeArquivo) {
    if (!arquivoExiste(nomeArquivo)) {
        throw std::runtime_error("Arquivo não encontrado: " + nomeArquivo);
    }
    
    std::ifstream arquivo(nomeArquivo);
    
    if (!arquivo.is_open()) {
        throw std::runtime_error("Erro ao abrir arquivo: " + nomeArquivo);
    }
    
    std::vector<int> numeros;
    std::string linha;
    
    // Lê primeira linha contendo a quantidade esperada
    if (!std::getline(arquivo, linha)) {
        throw std::runtime_error("Arquivo vazio ou formato inválido: " + nomeArquivo);
    }
    
    linha = trim(linha);
    size_t quantidadeEsperada = 0;
    
    try {
        quantidadeEsperada = std::stoull(linha);
    } catch (const std::exception& e) {
        throw std::runtime_error("Formato inválido na primeira linha: " + nomeArquivo);
    }
    
    if (quantidadeEsperada == 0) {
        throw std::runtime_error("Quantidade de números não pode ser zero");
    }
    
    // Pré-aloca memória para melhor performance
    numeros.reserve(quantidadeEsperada);
    size_t linhaAtual = 1;
    size_t numerosLidos = 0;
    
    // Processa o restante do arquivo
    while (std::getline(arquivo, linha) && numerosLidos < quantidadeEsperada) {
        linhaAtual++;
        linha = trim(linha);
        
        if (linha.empty()) {
            continue; // Ignora linhas vazias
        }
        
        try {
            int numero = std::stoi(linha);
            numeros.push_back(numero);
            numerosLidos++;
        } catch (const std::exception& e) {
            std::cerr << "Aviso: Linha " << linhaAtual 
                     << " inválida (\"" << linha << "\"), ignorando...\n";
        }
    }
    
    arquivo.close();
    
    if (numeros.empty()) {
        throw std::runtime_error("Nenhum número válido foi encontrado no arquivo");
    }
    
    return numeros;
}

/**
 * @brief Gera números aleatórios únicos
 * 
 * Para quantidades pequenas (até 10.000), garante unicidade.
 * Para quantidades maiores, usa geração com repetição.
 * 
 * @param quantidade Número de elementos únicos desejados
 * @return Vetor com números aleatórios únicos
 * @throws std::invalid_argument se quantidade for zero
 * 
 * @complexity O(n) amortizada
 */
std::vector<int> CarregadorDados::gerarNumerosAleatorios(size_t quantidade) {
    if (quantidade == 0) {
        throw std::invalid_argument("Quantidade deve ser maior que zero");
    }
    
    // Para grandes quantidades, usa geração com repetição
    if (quantidade > 10000) {
        return gerarNumerosAleatoriosComRepeticao(quantidade);
    }
    
    std::unordered_set<int> numerosUnicos;
    std::vector<int> resultado;
    resultado.reserve(quantidade);
    
    while (numerosUnicos.size() < quantidade) {
        int numero = distribuicao(gerador);
        if (numerosUnicos.insert(numero).second) {
            resultado.push_back(numero);
        }
    }
    
    return resultado;
}

/**
 * @brief Gera números aleatórios com possíveis repetições
 * 
 * Método otimizado para gerar grandes quantidades de números.
 * Conforme especificado no Trabalho 2, gera 1000 números aleatórios
 * entre 1 e 1.000.000 para testes de busca.
 * 
 * @param quantidade Número de elementos a gerar
 * @return Vetor com números aleatórios
 * @throws std::invalid_argument se quantidade for zero
 * 
 * @complexity O(n) linear
 */
std::vector<int> CarregadorDados::gerarNumerosAleatoriosComRepeticao(size_t quantidade) {
    if (quantidade == 0) {
        throw std::invalid_argument("Quantidade deve ser maior que zero");
    }
    
    std::vector<int> numeros;
    numeros.reserve(quantidade);
    
    for (size_t i = 0; i < quantidade; ++i) {
        numeros.push_back(distribuicao(gerador));
    }
    
    return numeros;
}

/**
 * @brief Salva dados em arquivo no formato padronizado
 * 
 * Salva no formato requerido:
 * - Primeira linha: quantidade total
 * - Linhas subsequentes: um número por linha
 * 
 * @param numeros Vetor com números a salvar
 * @param nomeArquivo Caminho do arquivo de destino
 * @return true se salvou com sucesso
 * 
 * @complexity O(n)
 */
bool CarregadorDados::salvarEmArquivo(const std::vector<int>& numeros, 
                                   const std::string& nomeArquivo) {
    if (numeros.empty()) {
        std::cerr << "Erro: Vetor vazio, não há dados para salvar.\n";
        return false;
    }
    
    // Cria estrutura de diretórios se necessário
    std::filesystem::path caminhoArquivo(nomeArquivo);
    if (caminhoArquivo.has_parent_path()) {
        std::filesystem::create_directories(caminhoArquivo.parent_path());
    }
    
    std::ofstream arquivo(nomeArquivo);
    
    if (!arquivo.is_open()) {
        throw std::runtime_error("Erro ao criar arquivo: " + nomeArquivo);
    }
    
    // Escreve formato padronizado
    arquivo << numeros.size() << "\n";
    
    for (const int& numero : numeros) {
        arquivo << numero << "\n";
    }
    
    arquivo.close();
    return true;
}

/**
 * @brief Valida formato e integridade de um arquivo
 * 
 * Verifica se o arquivo está no formato correto e é consistente.
 * 
 * @param nomeArquivo Caminho do arquivo a validar
 * @return true se arquivo é válido
 * 
 * @complexity O(n)
 */
bool CarregadorDados::validarArquivo(const std::string& nomeArquivo) const {
    try {
        if (!arquivoExiste(nomeArquivo)) {
            return false;
        }
        
        std::ifstream arquivo(nomeArquivo);
        std::string linha;
        
        if (!std::getline(arquivo, linha)) {
            return false; // Arquivo vazio
        }
        
        size_t quantidade = std::stoull(trim(linha));
        size_t contagem = 0;
        
        while (std::getline(arquivo, linha)) {
            linha = trim(linha);
            if (!linha.empty()) {
                try {
                    std::stoi(linha);
                    contagem++;
                } catch (...) {
                    return false; // Número inválido
                }
            }
        }
        
        return (contagem == quantidade);
        
    } catch (const std::exception& e) {
        return false;
    }
}

/**
 * @brief Exibe estatísticas básicas de um dataset
 * 
 * Carrega e analisa um dataset, exibindo informações relevantes.
 * 
 * @param nomeArquivo Arquivo a ser analisado
 * 
 * @complexity O(n)
 */
void CarregadorDados::exibirEstatisticas(const std::string& nomeArquivo) const {
    try {
        auto info = const_cast<CarregadorDados*>(this)->analisarDataset(nomeArquivo);
        
        std::cout << "\n=== Estatísticas do Dataset ===\n";
        std::cout << "Arquivo: " << info.nomeArquivo << "\n";
        std::cout << "Quantidade: " << info.quantidade << " números\n";
        std::cout << "Intervalo: [" << info.minimo << ", " << info.maximo << "]\n";
        std::cout << "Média: " << std::fixed << std::setprecision(2) << info.media << "\n";
        std::cout << "Duplicatas: " << (info.temDuplicatas ? "Sim" : "Não");
        if (info.temDuplicatas) {
            std::cout << " (" << info.numDuplicatas << " duplicatas)";
        }
        std::cout << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Erro ao analisar estatísticas: " << e.what() << "\n";
    }
}

/**
 * @brief Analisa um dataset e retorna informações estatísticas
 * 
 * @param nomeArquivo Arquivo a analisar
 * @return Estrutura InfoDataset com as informações
 * @throws std::runtime_error se não conseguir analisar
 * 
 * @complexity O(n)
 */
CarregadorDados::InfoDataset CarregadorDados::analisarDataset(const std::string& nomeArquivo) {
    InfoDataset info;
    info.nomeArquivo = nomeArquivo;
    
    auto numeros = carregarDeArquivo(nomeArquivo);
    info.quantidade = numeros.size();
    
    if (numeros.empty()) {
        throw std::runtime_error("Dataset vazio");
    }
    
    // Calcula mínimo e máximo
    info.minimo = *std::min_element(numeros.begin(), numeros.end());
    info.maximo = *std::max_element(numeros.begin(), numeros.end());
    
    // Calcula média com proteção contra overflow
    long long soma = std::accumulate(numeros.begin(), numeros.end(), 0LL);
    info.media = static_cast<double>(soma) / numeros.size();
    
    // Analisa duplicatas
    std::unordered_set<int> unicos;
    info.numDuplicatas = 0;
    
    for (int numero : numeros) {
        if (!unicos.insert(numero).second) {
            info.numDuplicatas++;
        }
    }
    
    info.temDuplicatas = (info.numDuplicatas > 0);
    
    return info;
}