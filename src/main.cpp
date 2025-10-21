/**
 * @file main.cpp
 * @brief Programa principal para análise comparativa de tabelas hash
 * 
 * Este arquivo implementa o programa principal que orquestra os benchmarks
 * comparativos entre diferentes implementações de tabelas hash (encadeamento
 * vs. endereçamento aberto) e diferentes funções de hash (divisão vs. multiplicação).
 * 
 * O programa:
 * 1. Carrega datasets de diferentes tamanhos
 * 2. Executa testes com múltiplas configurações
 * 3. Mede tempos de inserção e busca com precisão
 * 4. Calcula estatísticas de colisões e fator de carga
 * 5. Gera relatórios em console e arquivo CSV
 * 
 * @author Gabriel Freitas Souza
 * @author Roberli Schuina Silva
 * @date 2024-10-18
 * @version 1.1
 * 
 * Disciplina: Pesquisa e Ordenação
 * Projeto: Análise Comparativa de Soluções Baseadas em Hashing
 */

#include <iostream>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <limits>
#include <cmath>

#include "TabelaEncadeada.hpp"
#include "TabelaAberta.hpp"
#include "CarregadorDados.hpp"

/**
 * @brief Estrutura para armazenar resultados de um teste específico
 * 
 * Cada instância representa o resultado de um cenário de teste,
 * incluindo configuração usada e métricas coletadas.
 * 
 * Esta estrutura é usada para:
 * - Armazenar resultados temporariamente durante execução
 * - Gerar relatórios formatados
 * - Exportar dados para análise posterior (CSV)
 */
struct ResultadoTeste {
    std::string tipoTabela;      ///< "Encadeada" ou "Aberta"
    size_t tamanhoTabela;        ///< Tamanho da tabela hash utilizada
    size_t quantidadeDados;      ///< Número de elementos inseridos
    std::string tipoFuncaoHash;  ///< "Divisao" ou "Multiplicacao"
    double tempoInsercao;        ///< Tempo de inserção em milissegundos
    double tempoBusca;           ///< Tempo de busca em milissegundos
    size_t colisoes;             ///< Número estimado de colisões
    double fatorCarga;           ///< Fator de carga (elementos/tamanho)
};

/**
 * @brief Classe gerenciadora de benchmarks
 * 
 * Responsável por:
 * - Executar testes sistemáticos nas tabelas hash
 * - Medir performance com precisão usando std::chrono
 * - Calcular estatísticas de colisões
 * - Gerar relatórios formatados
 * - Exportar resultados para arquivo CSV
 * 
 * A classe utiliza templates para medição genérica de tempo,
 * permitindo benchmarking de qualquer função lambda.
 */
class BenchmarkManager {
private:
    std::vector<ResultadoTeste> resultados;  ///< Armazena todos os resultados dos testes

    /**
     * @brief Template genérico para medição precisa de tempo
     * @tparam Func Tipo da função a ser medida
     * @param func Função lambda a ser executada e medida
     * @return Tempo de execução em milissegundos
     * 
     * Utiliza std::chrono::high_resolution_clock para máxima precisão.
     * A conversão para milissegundos facilita interpretação dos resultados.
     * 
     * @complexity O(1) + complexidade da função medida
     */
    template<typename Func>
    double medirTempo(Func&& func) {
        auto inicio = std::chrono::high_resolution_clock::now();
        func();  // Executa a função a ser medida
        auto fim = std::chrono::high_resolution_clock::now();
        
        // Converte para milissegundos com precisão decimal
        auto duracao = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio);
        return duracao.count() / 1000.0;
    }

    /**
     * @brief Calcula estimativa de colisões para tabela encadeada
     * @param tabela Referência para a tabela encadeada
     * @return Número estimado de colisões
     * 
     * Para tabelas com encadeamento, usa aproximação baseada no
     * fator de carga λ = n/m:
     * - Se λ ≤ 1: colisões ≈ n - m(1 - e^(-λ))
     * - Se λ > 1: colisões ≈ n - m (saturação)
     * 
     * Esta fórmula deriva da distribuição de Poisson para
     * o número de elementos por posição.
     */
    size_t contarColisoesEncadeada(const TabelaEncadeada& tabela) {
        size_t elementos = tabela.getNumElementos();
        size_t tamanho = tabela.getTamanho();
        
        if (elementos <= tamanho) {
            double lambda = static_cast<double>(elementos) / tamanho;
            // Aproximação: colisões = n - m * (1 - e^(-λ))
            return static_cast<size_t>(elementos - tamanho * (1 - std::exp(-lambda)));
        }
        
        // Para fator de carga > 1, número de colisões ≈ elementos - tamanho
        return elementos - tamanho;
    }

    /**
     * @brief Calcula estimativa de colisões para tabela aberta
     * @param tabela Referência para a tabela aberta
     * @return Número estimado de colisões
     * 
     * Para endereçamento aberto com sondagem linear:
     * - Colisões estimadas ≈ n * fc / 2
     * - Onde fc é o fator de carga
     * 
     * Esta estimativa considera o clustering primário
     * típico da sondagem linear.
     */
    size_t contarColisoesAberta(const TabelaAberta& tabela) {
        size_t elementos = tabela.getNumElementos();
        size_t tamanho = tabela.getTamanho();
        
        if (elementos <= tamanho) {
            double fc = tabela.fatorCarga();
            // Aproximação considerando clustering primário
            return static_cast<size_t>(elementos * fc / 2.0);
        }
        
        return elementos; // Caso extremo: tabela cheia
    }

public:
    /**
     * @brief Executa testes completos na tabela encadeada
     * @param dados Dataset para inserção
     * @param dadosBusca Dataset para busca
     * @param tamanhoTabela Tamanho da tabela a ser testada
     * 
     * Executa dois testes completos:
     * 1. Com função hash de divisão
     * 2. Com função hash de multiplicação
     * 
     * Para cada teste:
     * - Cria nova instância da tabela
     * - Mede tempo de inserção de todos os elementos
     * - Mede tempo de busca de elementos do dataset de busca
     * - Calcula estatísticas (colisões, fator de carga)
     * - Armazena resultados para relatório
     * 
     * @complexity O(n + b) onde n é tamanho de dados e b é tamanho de dadosBusca
     */
    void testarTabelaEncadeada(const std::vector<int>& dados,
                              const std::vector<int>& dadosBusca,
                              size_t tamanhoTabela) {
        std::cout << "  Testando tabela encadeada (tamanho: " << tamanhoTabela << ")...";

        // Teste com função hash de divisão
        {
            TabelaEncadeada tabela(tamanhoTabela);
            
            // Mede tempo de inserção
            double tempoInsercao = medirTempo([&]() { 
                for (int valor : dados) {
                    tabela.inserir(valor, TabelaEncadeada::TipoHash::DIVISAO);
                }
            });
            
            // Mede tempo de busca
            double tempoBusca = medirTempo([&]() { 
                for (int valor : dadosBusca) {
                    tabela.buscar(valor, TabelaEncadeada::TipoHash::DIVISAO);
                }
            });
            
            // Armazena resultado
            resultados.push_back({
                "Encadeada",
                tamanhoTabela,
                dados.size(),
                "Divisao",
                tempoInsercao,
                tempoBusca,
                contarColisoesEncadeada(tabela),
                tabela.fatorCarga()
            });
        }
        
        // Teste com função hash de multiplicação
        {
            TabelaEncadeada tabela(tamanhoTabela);
            
            double tempoInsercao = medirTempo([&]() { 
                for (int valor : dados) {
                    tabela.inserir(valor, TabelaEncadeada::TipoHash::MULTIPLICACAO);
                }
            });
            
            double tempoBusca = medirTempo([&]() { 
                for (int valor : dadosBusca) {
                    tabela.buscar(valor, TabelaEncadeada::TipoHash::MULTIPLICACAO);
                }
            });
            
            resultados.push_back({
                "Encadeada",
                tamanhoTabela,
                dados.size(),
                "Multiplicacao",
                tempoInsercao,
                tempoBusca,
                contarColisoesEncadeada(tabela),
                tabela.fatorCarga()
            });
        }
        
        std::cout << " OK" << std::endl;
    }

    /**
     * @brief Executa testes completos na tabela aberta
     * @param dados Dataset para inserção
     * @param dadosBusca Dataset para busca
     * 
     * Similar ao teste da tabela encadeada, mas:
     * - Usa tamanho fixo (50009) para evitar problemas de fator de carga
     * - Inclui tratamento de exceções para tabela cheia
     * - Para inserção se atingir limite de capacidade
     * 
     * O tamanho 50009 é primo e suficientemente grande para
     * todos os datasets testados, mantendo fator de carga < 1.
     * 
     * @complexity O(n + b) onde n é tamanho de dados e b é tamanho de dadosBusca
     */
    void testarTabelaAberta(const std::vector<int>& dados,
                           const std::vector<int>& dadosBusca) {
        const size_t TAM = 50009; // Número primo para melhor distribuição
        std::cout << "  Testando tabela aberta (tamanho: " << TAM << ")...";
        
        // Teste com função hash de divisão
        {
            TabelaAberta tabela(TAM);
            
            double tempoInsercao = medirTempo([&]() {
                for (int valor : dados) {
                    try {
                        tabela.inserir(valor, TabelaAberta::TipoHash::DIVISAO);
                    } catch (const std::runtime_error&) {
                        // Para se tabela cheia ou fator de carga muito alto
                        break;
                    }
                }
            });
            
            double tempoBusca = medirTempo([&]() { 
                for (int valor : dadosBusca) {
                    tabela.buscar(valor, TabelaAberta::TipoHash::DIVISAO);
                }
            });
            
            resultados.push_back({
                "Aberta",
                TAM,
                tabela.getNumElementos(), // Pode ser menor que dados.size() se houve overflow
                "Divisao",
                tempoInsercao,
                tempoBusca,
                contarColisoesAberta(tabela),
                tabela.fatorCarga()
            });
        }
        
        // Teste com função hash de multiplicação
        {
            TabelaAberta tabela(TAM);
            
            double tempoInsercao = medirTempo([&]() {
                for (int valor : dados) {
                    try {
                        tabela.inserir(valor, TabelaAberta::TipoHash::MULTIPLICACAO);
                    } catch (const std::runtime_error&) {
                        break;
                    }
                }
            });
            
            double tempoBusca = medirTempo([&]() { 
                for (int valor : dadosBusca) {
                    tabela.buscar(valor, TabelaAberta::TipoHash::MULTIPLICACAO);
                }
            });
            
            resultados.push_back({
                "Aberta",
                TAM,
                tabela.getNumElementos(),
                "Multiplicacao",
                tempoInsercao,
                tempoBusca,
                contarColisoesAberta(tabela),
                tabela.fatorCarga()
            });
        }
        
        std::cout << " OK" << std::endl;
    }

    /**
     * @brief Salva todos os resultados em arquivo CSV
     * @param arquivo Caminho do arquivo de saída
     * @throws std::runtime_error se não conseguir criar o arquivo
     * 
     * Formato CSV:
     * - Cabeçalho com nomes das colunas
     * - Uma linha por resultado de teste
     * - Campos separados por vírgula
     * - Números formatados com precisão apropriada
     * 
     * O arquivo gerado pode ser:
     * - Importado em planilhas (Excel, LibreOffice)
     * - Usado para gerar gráficos automáticos
     * - Processado por scripts de análise
     * 
     * @complexity O(r) onde r é o número de resultados
     */
    void salvarResultados(const std::string& arquivo) {
        std::ofstream arq(arquivo);
        if (!arq.is_open()) {
            throw std::runtime_error("Erro ao criar arquivo: " + arquivo);
        }
        
        // Escreve cabeçalho CSV
        arq << "TipoTabela,TamanhoTabela,QuantidadeDados,FuncaoHash,"
            << "TempoInsercao(ms),TempoBusca(ms),Colisoes,FatorCarga\n";
        
        // Escreve dados formatados
        for (const auto& resultado : resultados) {
            arq << resultado.tipoTabela << ","
                << resultado.tamanhoTabela << ","
                << resultado.quantidadeDados << ","
                << resultado.tipoFuncaoHash << ","
                << std::fixed << std::setprecision(3) << resultado.tempoInsercao << ","
                << std::setprecision(3) << resultado.tempoBusca << ","
                << resultado.colisoes << ","
                << std::setprecision(4) << resultado.fatorCarga << "\n";
        }
        
        arq.close();
        std::cout << "\nResultados salvos em: " << arquivo << std::endl;
    }

    /**
     * @brief Imprime relatório formatado no console
     * 
     * Gera tabela organizada com:
     * - Cabeçalho descritivo
     * - Colunas alinhadas
     * - Números formatados para legibilidade
     * - Separadores visuais
     * 
     * O relatório permite análise rápida dos resultados
     * sem necessidade de abrir arquivo externo.
     * 
     * @complexity O(r) onde r é o número de resultados
     */
    void imprimirRelatorio() {
        if (resultados.empty()) {
            std::cout << "Nenhum resultado disponível." << std::endl;
            return;
        }
        
        // Cabeçalho do relatório
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "RELATÓRIO DE PERFORMANCE" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        // Cabeçalho da tabela
        std::cout << std::left
                  << std::setw(10) << "Tipo"
                  << std::setw(8)  << "Tam.Tab"
                  << std::setw(8)  << "Dados"
                  << std::setw(12) << "Hash"
                  << std::setw(12) << "Inser.(ms)"
                  << std::setw(12) << "Busca(ms)"
                  << std::setw(8)  << "Colisões"
                  << std::setw(8)  << "F.Carga" << std::endl;
        
        std::cout << std::string(80, '-') << std::endl;
        
        // Dados da tabela
        for (const auto& resultado : resultados) {
            std::cout << std::left
                      << std::setw(10) << resultado.tipoTabela
                      << std::setw(8)  << resultado.tamanhoTabela
                      << std::setw(8)  << resultado.quantidadeDados
                      << std::setw(12) << resultado.tipoFuncaoHash
                      << std::setw(12) << std::fixed << std::setprecision(3) << resultado.tempoInsercao
                      << std::setw(12) << std::fixed << std::setprecision(3) << resultado.tempoBusca
                      << std::setw(8)  << resultado.colisoes
                      << std::setw(8)  << std::fixed << std::setprecision(4) << resultado.fatorCarga
                      << std::endl;
        }
        
        std::cout << std::string(80, '=') << std::endl;
    }
};

/**
 * @brief Pausa o console em sistemas Windows
 * 
 * Função utilitária para evitar que a janela do console
 * feche automaticamente em sistemas Windows quando o
 * programa é executado via duplo-clique.
 * 
 * Em outros sistemas, não faz nada.
 */
static void pause_console() {
#ifdef _WIN32
    std::cout << "\nPressione ENTER para sair...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
#endif
}

/**
 * @brief Função principal do programa
 * @param argc Número de argumentos da linha de comando (não utilizado)
 * @param argv Array de argumentos da linha de comando (não utilizado)
 * @return 0 se execução bem-sucedida, 1 se erro
 * 
 * Fluxo principal:
 * 1. Inicialização de componentes (carregador, benchmark manager)
 * 2. Definição de parâmetros de teste
 * 3. Geração de dataset para busca
 * 4. Loop principal: para cada arquivo de dataset
 *    a. Carrega dados
 *    b. Executa testes em todas as configurações
 *    c. Armazena resultados
 * 5. Gera relatórios (console + CSV)
 * 6. Finalização com tratamento de erros
 * 
 * Tratamento robusto de erros:
 * - Exceções específicas para problemas conhecidos
 * - Exceções genéricas para erros inesperados
 * - Continuação da execução mesmo com falhas individuais
 * - Pausa do console em Windows para visualização
 * 
 * @complexity O(k * n * t) onde:
 * - k = número de arquivos de dataset
 * - n = tamanho médio dos datasets
 * - t = número de configurações testadas
 */
int main(int /*argc*/, char* /*argv*/[]) {
    try {
        // Cabeçalho do programa
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "ANÁLISE COMPARATIVA DE TABELAS HASH" << std::endl;
        std::cout << std::string(60, '=') << std::endl;

        // Inicialização dos componentes principais
        CarregadorDados carregador;
        BenchmarkManager benchmark;

        // Configuração dos tamanhos de tabela encadeada (números primos)
        const std::vector<size_t> TAM_TABELA_ENCADEADA = {29, 97, 251, 499, 911};
        
        // Lista de arquivos de dataset conforme especificação do Trabalho 2
        const std::vector<std::string> ARQUIVOS = {
            "data/numeros_aleatorios_100.txt",
            "data/numeros_aleatorios_500.txt",
            "data/numeros_aleatorios_1000.txt",
            "data/numeros_aleatorios_5000.txt",
            "data/numeros_aleatorios_10000.txt",
            "data/numeros_aleatorios_50000.txt"
        };

        // Geração de dataset para operações de busca (1000 números aleatórios entre 1 e 1.000.000)
        std::cout << "\nGerando dados para busca (1000 números aleatórios entre 1 e 1.000.000)...";
        auto dadosBusca = carregador.gerarNumerosAleatoriosComRepeticao(1000);
        std::cout << " OK\n";

        // Loop principal: testa cada arquivo de dataset
        for (const std::string& arquivo : ARQUIVOS) {
            try {
                std::cout << "\nCarregando dados de: " << arquivo << std::endl;
                auto dados = carregador.carregarDeArquivo(arquivo);
                
                std::cout << "Executando testes com " << dados.size() << " elementos:" << std::endl;
                
                // Testa todas as configurações de tabela encadeada
                for (size_t tamanho : TAM_TABELA_ENCADEADA) {
                    benchmark.testarTabelaEncadeada(dados, dadosBusca, tamanho);
                }
                
                // Testa tabela aberta (tamanho fixo)
                benchmark.testarTabelaAberta(dados, dadosBusca);
                
            } catch (const std::exception& e) {
                std::cerr << "Erro ao processar arquivo " << arquivo << ": " 
                          << e.what() << std::endl;
                continue; // Continua com próximo arquivo
            }
        }

        // Geração de relatórios
        benchmark.imprimirRelatorio();
        benchmark.salvarResultados("resultados_benchmark.csv");

        std::cout << "\nAnálise concluída com sucesso!\n" << std::endl;
        pause_console();
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Erro crítico: " << e.what() << std::endl;
        pause_console();
        return 1;
        
    } catch (...) {
        std::cerr << "Erro desconhecido ocorreu." << std::endl;
        pause_console();
        return 1;
    }
}