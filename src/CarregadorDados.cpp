/**
 * @file CarregadorDados.cpp
 * @brief Implementação da classe CarregadorDados
 * 
 * Este arquivo contém a implementação completa de todos os métodos da classe
 * CarregadorDados, incluindo carregamento de arquivos, geração de dados
 * aleatórios e análise estatística.
 * 
 * @author Gabriel Freitas Souza
 * @author Roberli Schuina Silva
 * @date 2024-10-18
 * @version 1.0
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
 * @brief Implementação do carregamento de dados de arquivo
 * 
 * Esta função implementa o carregamento robusto de datasets de arquivos
 * de texto. O algoritmo segue o formato padronizado do projeto:
 * 1. Primeira linha contém a quantidade de números
 * 2. Linhas subsequentes contêm um número por linha
 * 3. Linhas vazias são ignoradas
 * 4. Tratamento de erros de conversão
 * 
 * O carregamento é otimizado com pré-alocação de memória baseada
 * na quantidade esperada, melhorando a performance para datasets grandes.
 * 
 * @param nomeArquivo Caminho para o arquivo de dados
 * @return Vetor com números carregados
 * @throws std::runtime_error se arquivo inacessível ou formato inválido
 * 
 * @complexity O(n) onde n é o número de linhas no arquivo
 */
std::vector<int> CarregadorDados::carregarDeArquivo(const std::string& nomeArquivo) {
    // Validação inicial de existência
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
    
    // Converte primeira linha para quantidade
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
        
        // Ignora linhas vazias para flexibilidade de formato
        if (linha.empty()) {
            continue;
        }
        
        try {
            int numero = std::stoi(linha);
            numeros.push_back(numero);
            numerosLidos++;
        } catch (const std::exception& e) {
            // Log do erro mas continua processamento para robustez
            std::cerr << "Aviso: Linha " << linhaAtual 
                     << " inválida (\"" << linha << "\"), ignorando...\n";
        }
    }
    
    arquivo.close();
    
    // Valida consistência entre quantidade esperada e lida
    if (numeros.size() != quantidadeEsperada) {
        std::cerr << "Aviso: Esperado " << quantidadeEsperada 
                 << " números, mas leu " << numeros.size() << "\n";
    }
    
    if (numeros.empty()) {
        throw std::runtime_error("Nenhum número válido foi encontrado no arquivo");
    }
    
    std::cout << "✓ Arquivo carregado: " << nomeArquivo 
              << " (" << numeros.size() << " números)\n";
    
    return numeros;
}

/**
 * @brief Implementação da geração de números aleatórios únicos
 * 
 * Para quantidades pequenas (até 10.000), garante unicidade usando
 * std::unordered_set. Para quantidades maiores, delega para geração
 * com repetição por questões de performance.
 * 
 * O uso de unordered_set oferece inserção O(1) amortizada e
 * detecção de duplicatas eficiente.
 * 
 * @param quantidade Número de elementos únicos desejados
 * @return Vetor com números aleatórios únicos
 * @throws std::invalid_argument se quantidade for zero
 * 
 * @complexity O(n) amortizada, onde n é a quantidade desejada
 */
std::vector<int> CarregadorDados::gerarNumerosAleatorios(size_t quantidade) {
    if (quantidade == 0) {
        throw std::invalid_argument("Quantidade deve ser maior que zero");
    }
    
    // Para grandes quantidades, usar geração com repetição para evitar
    // problemas de performance na detecção de duplicatas
    if (quantidade > 10000) {
        return gerarNumerosAleatoriosComRepeticao(quantidade);
    }
    
    // Usa hash set para detecção eficiente de duplicatas
    std::unordered_set<int> numerosUnicos;
    std::vector<int> resultado;
    resultado.reserve(quantidade);
    
    // Gera números até atingir quantidade desejada
    while (numerosUnicos.size() < quantidade) {
        int numero = distribuicao(gerador);
        // insert retorna pair<iterator, bool> - second indica se foi inserido
        if (numerosUnicos.insert(numero).second) {
            resultado.push_back(numero);
        }
    }
    
    return resultado;
}

/**
 * @brief Implementação da geração de números aleatórios com repetição
 * 
 * Versão mais simples e rápida que permite duplicatas.
 * Ideal para datasets grandes ou quando duplicatas são aceitáveis.
 * 
 * A pré-alocação de memória melhora a performance evitando
 * realocações durante o crescimento do vetor.
 * 
 * @param quantidade Número de elementos a gerar
 * @return Vetor com números aleatórios (pode conter duplicatas)
 * @throws std::invalid_argument se quantidade for zero
 * 
 * @complexity O(n) linear
 */
std::vector<int> CarregadorDados::gerarNumerosAleatoriosComRepeticao(size_t quantidade) {
    if (quantidade == 0) {
        throw std::invalid_argument("Quantidade deve ser maior que zero");
    }
    
    std::vector<int> numeros;
    numeros.reserve(quantidade); // Pré-alocação para eficiência
    
    // Geração direta sem verificação de duplicatas
    for (size_t i = 0; i < quantidade; ++i) {
        numeros.push_back(distribuicao(gerador));
    }
    
    return numeros;
}

/**
 * @brief Implementação do salvamento de dados em arquivo
 * 
 * Salva dados no formato padronizado do projeto:
 * - Primeira linha: quantidade total
 * - Linhas subsequentes: um número por linha
 * 
 * Cria automaticamente diretórios pai se necessário,
 * garantindo que o arquivo possa ser salvo mesmo que
 * a estrutura de diretórios não exista.
 * 
 * @param numeros Vetor com números a salvar
 * @param nomeArquivo Caminho do arquivo de destino
 * @return true se salvou com sucesso, false caso contrário
 * 
 * @complexity O(n) onde n é o tamanho do vetor
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
    
    // Escreve formato padronizado: quantidade na primeira linha
    arquivo << numeros.size() << "\n";
    
    // Escreve cada número em uma linha separada
    for (const int& numero : numeros) {
        arquivo << numero << "\n";
    }
    
    arquivo.close();
    
    std::cout << "✓ Arquivo salvo: " << nomeArquivo 
              << " (" << numeros.size() << " números)\n";
    
    return true;
}

/**
 * @brief Implementação da validação de arquivos
 * 
 * Realiza validação completa da integridade do arquivo:
 * 1. Existência e acessibilidade
 * 2. Formato da primeira linha (quantidade)
 * 3. Consistência entre quantidade declarada e números presentes
 * 4. Validade de cada número individualmente
 * 
 * @param nomeArquivo Caminho do arquivo a validar
 * @return true se arquivo é válido, false caso contrário
 * 
 * @complexity O(n) onde n é o número de linhas
 */
bool CarregadorDados::validarArquivo(const std::string& nomeArquivo) const {
    try {
        if (!arquivoExiste(nomeArquivo)) {
            std::cerr << "Arquivo não existe: " << nomeArquivo << "\n";
            return false;
        }
        
        std::ifstream arquivo(nomeArquivo);
        std::string linha;
        
        // Valida primeira linha (quantidade)
        if (!std::getline(arquivo, linha)) {
            std::cerr << "Arquivo vazio\n";
            return false;
        }
        
        size_t quantidade = std::stoull(trim(linha));
        size_t contagem = 0;
        
        // Conta e valida cada número no arquivo
        while (std::getline(arquivo, linha)) {
            linha = trim(linha);
            if (!linha.empty()) {
                try {
                    std::stoi(linha); // Tenta converter para validar formato
                    contagem++;
                } catch (...) {
                    std::cerr << "Número inválido na linha " << (contagem + 2) << "\n";
                    return false;
                }
            }
        }
        
        // Verifica consistência entre quantidade declarada e encontrada
        if (contagem != quantidade) {
            std::cerr << "Quantidade esperada: " << quantidade 
                     << ", encontrada: " << contagem << "\n";
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Erro ao validar arquivo: " << e.what() << "\n";
        return false;
    }
}

/**
 * @brief Implementação da exibição de estatísticas
 * 
 * Carrega e analisa um dataset, exibindo estatísticas formatadas:
 * - Informações básicas (nome, quantidade)
 * - Estatísticas descritivas (intervalo, média)
 * - Análise de qualidade (duplicatas)
 * 
 * Utiliza const_cast para manter interface const enquanto
 * permite chamada de métodos não-const necessários.
 * 
 * @param nomeArquivo Arquivo a ser analisado
 * 
 * @complexity O(n) onde n é o número de elementos no dataset
 */
void CarregadorDados::exibirEstatisticas(const std::string& nomeArquivo) const {
    try {
        // Analisa dataset usando método auxiliar
        auto info = const_cast<CarregadorDados*>(this)->analisarDataset(nomeArquivo);
        
        // Exibe relatório formatado
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
 * @brief Implementação da análise detalhada de dataset
 * 
 * Carrega um dataset e calcula estatísticas completas:
 * - Valores mínimo e máximo (usando std::min_element/max_element)
 * - Média aritmética (usando std::accumulate com overflow protection)
 * - Análise de duplicatas (usando std::unordered_set)
 * 
 * @param nomeArquivo Arquivo a analisar
 * @return Estrutura InfoDataset com as informações
 * @throws std::runtime_error se não conseguir analisar
 * 
 * @complexity O(n) onde n é o número de elementos
 */
CarregadorDados::InfoDataset CarregadorDados::analisarDataset(const std::string& nomeArquivo) {
    InfoDataset info;
    info.nomeArquivo = nomeArquivo;
    
    auto numeros = carregarDeArquivo(nomeArquivo);
    info.quantidade = numeros.size();
    
    if (numeros.empty()) {
        throw std::runtime_error("Dataset vazio");
    }
    
    // Calcula mínimo e máximo em uma passada
    info.minimo = *std::min_element(numeros.begin(), numeros.end());
    info.maximo = *std::max_element(numeros.begin(), numeros.end());
    
    // Calcula média com proteção contra overflow
    long long soma = std::accumulate(numeros.begin(), numeros.end(), 0LL);
    info.media = static_cast<double>(soma) / numeros.size();
    
    // Analisa duplicatas usando hash set para eficiência
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

/**
 * @brief Implementação do relatório de datasets
 * 
 * Gera relatório consolidado de todos os datasets disponíveis
 * na pasta data/, mostrando estatísticas de cada um em formato
 * tabular para fácil comparação.
 * 
 * @complexity O(k*n) onde k é o número de arquivos e n a média de elementos
 */
void CarregadorDados::gerarRelatorioDatasets() const {
    auto arquivos = listarArquivosDisponiveis();
    
    if (arquivos.empty()) {
        std::cout << "Nenhum arquivo encontrado no diretório data/\n";
        return;
    }
    
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "                       RELATÓRIO DOS DATASETS\n";
    std::cout << std::string(80, '=') << "\n";
    
    // Analisa cada arquivo encontrado
    for (const std::string& arquivo : arquivos) {
        try {
            auto info = const_cast<CarregadorDados*>(this)->analisarDataset(arquivo);
            
            std::cout << "\nArquivo: " << std::filesystem::path(arquivo).filename() << "\n";
            std::cout << "  Elementos: " << info.quantidade << "\n";
            std::cout << "  Intervalo: [" << info.minimo << ", " << info.maximo << "]\n";
            std::cout << "  Média: " << std::fixed << std::setprecision(2) << info.media << "\n";
            std::cout << "  Duplicatas: " << (info.temDuplicatas ? "Sim" : "Não");
            if (info.temDuplicatas) {
                std::cout << " (" << info.numDuplicatas << ")"; 
            }
            std::cout << "\n";
            
        } catch (const std::exception& e) {
            std::cout << "\nErro ao analisar " 
                     << std::filesystem::path(arquivo).filename() 
                     << ": " << e.what() << "\n";
        }
    }
    
    std::cout << std::string(80, '=') << "\n";
}

/**
 * @brief Implementação da geração de arquivos para o trabalho
 * 
 * Gera automaticamente todos os arquivos necessários para os
 * testes do trabalho, seguindo as quantidades especificadas.
 * 
 * Arquivos gerados:
 * - 6 arquivos para inserção (100, 500, 1000, 5000, 10000, 50000)
 * - 1 arquivo para busca (1000 números)
 * 
 * @param diretorio Diretório de destino (padrão: "data")
 * @throws std::runtime_error se não conseguir gerar algum arquivo
 * 
 * @complexity O(sum(quantidades)) - linear na soma das quantidades
 */
void CarregadorDados::gerarArquivosTrabalho(const std::string& diretorio) {
    std::cout << "\n=== Gerando Arquivos de Dados ===\n";
    
    // Cria diretório se não existir
    std::filesystem::create_directories(diretorio);
    
    // Quantidades conforme especificação do trabalho
    std::vector<size_t> quantidades = {100, 500, 1000, 5000, 10000, 50000};
    
    try {
        // Gera arquivos de dados para inserção
        for (size_t quantidade : quantidades) {
            std::string nomeArquivo = diretorio + "/numeros_aleatorios_" + 
                                     std::to_string(quantidade) + ".txt";
            
            std::cout << "Gerando " << quantidade << " números...\n";
            auto numeros = gerarNumerosAleatoriosComRepeticao(quantidade);
            salvarEmArquivo(numeros, nomeArquivo);
        }
        
        // Gera arquivo adicional para busca
        std::cout << "\nGerando 1000 números para busca...\n";
        auto numerosBusca = gerarNumerosAleatoriosComRepeticao(1000);
        salvarEmArquivo(numerosBusca, diretorio + "/busca_1000.txt");
        
        std::cout << "\n✓ Todos os arquivos foram gerados com sucesso!\n";
        std::cout << "Diretório: " << diretorio << "\n\n";
        
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Erro ao gerar arquivos: ") + e.what());
    }
}

/**
 * @brief Implementação do teste de desempenho
 * 
 * Método estático que benchmarca diferentes operações da classe:
 * - Geração com repetição (mais rápida)
 * - Geração única (mais lenta mas sem duplicatas)
 * 
 * Utiliza std::chrono para medições precisas de tempo.
 * Limita testes de unicidade a quantidades menores para
 * evitar tempos de execução muito longos.
 * 
 * @param quantidades Vetor com tamanhos a testar
 * 
 * @complexity Varia conforme as operações testadas
 */
void BenchmarkCarregadorDados::testarDesempenho(const std::vector<size_t>& quantidades) {
    std::cout << "\n=== Benchmark de Carregamento e Geração de Dados ===\n";
    
    CarregadorDados carregador;
    
    for (size_t quantidade : quantidades) {
        std::cout << "\nTestando com " << quantidade << " elementos:\n";
        
        // Testa geração com repetição (sempre rápida)
        auto inicio = std::chrono::high_resolution_clock::now();
        auto dados = carregador.gerarNumerosAleatoriosComRepeticao(quantidade);
        auto fim = std::chrono::high_resolution_clock::now();
        
        auto duracao = std::chrono::duration_cast<std::chrono::milliseconds>(fim - inicio);
        
        std::cout << "  Geração com repetição: " << duracao.count() << "ms\n";
        
        // Testa geração única (apenas para quantidades menores)
        if (quantidade <= 10000) {
            inicio = std::chrono::high_resolution_clock::now();
            auto dadosUnicos = carregador.gerarNumerosAleatorios(quantidade);
            fim = std::chrono::high_resolution_clock::now();
            
            duracao = std::chrono::duration_cast<std::chrono::milliseconds>(fim - inicio);
            std::cout << "  Geração única: " << duracao.count() << "ms\n";
        }
    }
}