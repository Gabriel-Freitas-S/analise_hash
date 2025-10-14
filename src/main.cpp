#include <iostream>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>

#include "TabelaHashEncadeada.hpp"
#include "TabelaHashAberta.hpp"
#include "GeradorDados.hpp"

/**
 * @brief Estrutura para armazenar resultados de testes de performance
 */
struct ResultadoTeste {
    std::string tipoTabela;          ///< "Encadeada" ou "Aberta"
    size_t tamanhoTabela;            ///< Tamanho da tabela hash
    size_t quantidadeDados;          ///< Número de elementos inseridos
    std::string tipoFuncaoHash;      ///< "Divisao" ou "Multiplicacao"
    double tempoInsercao;            ///< Tempo de inserção em milissegundos
    double tempoBusca;               ///< Tempo de busca em milissegundos
    size_t colisoes;                 ///< Número de colisões ocorridas
    double fatorCarga;               ///< Fator de carga da tabela
};

/**
 * @brief Classe responsável por executar e gerenciar benchmarks das tabelas hash
 */
class BenchmarkManager {
private:
    std::vector<ResultadoTeste> resultados;
    
    /**
     * @brief Mede o tempo de execução de uma função
     * @param func Função a ser cronometrada
     * @return Tempo de execução em milissegundos
     */
    template<typename Func>
    double medirTempo(Func&& func) {
        auto inicio = std::chrono::high_resolution_clock::now();
        func();
        auto fim = std::chrono::high_resolution_clock::now();
        auto duracao = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio);
        return duracao.count() / 1000.0; // Converte para milissegundos com precisão
    }
    
    /**
     * @brief Conta o número de colisões em uma tabela encadeada
     * @param tabela Referência para a tabela hash encadeada
     * @return Número estimado de colisões
     */
    size_t contarColisoesEncadeada(const TabelaHashEncadeada& tabela) {
        // Estimativa baseada no fator de carga e distribuição
        size_t elementos = tabela.getNumElementos();
        size_t tamanho = tabela.getTamanho();
        
        if (elementos <= tamanho) {
            return 0;
        }
        
        // Aproximação usando a fórmula de colisões esperadas
        double lambda = static_cast<double>(elementos) / tamanho;
        return static_cast<size_t>(elementos - tamanho * (1 - std::exp(-lambda)));
    }
    
    /**
     * @brief Conta o número de colisões em uma tabela aberta
     * @param tabela Referência para a tabela hash aberta
     * @return Número de colisões (estimativa baseada em sondagem)
     */
    size_t contarColisoesAberta(const TabelaHashAberta& tabela) {
        // Para tabela aberta, o número de colisões pode ser estimado
        // baseado no fator de carga e na distribuição
        size_t elementos = tabela.getNumElementos();
        size_t tamanho = tabela.getTamanho();
        
        if (elementos <= tamanho) {
            double fatorCarga = tabela.fatorCarga();
            // Estimativa usando teoria de hashing
            return static_cast<size_t>(elementos * fatorCarga / 2.0);
        }
        
        return elementos; // Se a tabela estiver cheia, muitas colisões
    }
    
public:
    /**
     * @brief Testa o desempenho de tabelas hash encadeadas
     * @param dados Vetor com dados para inserção
     * @param dadosBusca Vetor com dados para busca
     * @param tamanhoTabela Tamanho da tabela hash
     */
    void testarTabelaEncadeada(const std::vector<int>& dados,
                              const std::vector<int>& dadosBusca,
                              size_t tamanhoTabela) {
        std::cout << "  Testando tabela encadeada (tamanho: " << tamanhoTabela << ")...";
        
        // Teste com função de divisão
        {
            TabelaHashEncadeada tabela(tamanhoTabela);
            
            double tempoInsercao = medirTempo([&]() {
                for (int valor : dados) {
                    tabela.inserir(valor, TabelaHashEncadeada::TipoHash::DIVISAO);
                }
            });
            
            double tempoBusca = medirTempo([&]() {
                for (int valor : dadosBusca) {
                    tabela.buscar(valor, TabelaHashEncadeada::TipoHash::DIVISAO);
                }
            });
            
            ResultadoTeste resultado;
            resultado.tipoTabela = "Encadeada";
            resultado.tamanhoTabela = tamanhoTabela;
            resultado.quantidadeDados = dados.size();
            resultado.tipoFuncaoHash = "Divisao";
            resultado.tempoInsercao = tempoInsercao;
            resultado.tempoBusca = tempoBusca;
            resultado.colisoes = contarColisoesEncadeada(tabela);
            resultado.fatorCarga = tabela.fatorCarga();
            
            resultados.push_back(resultado);
        }
        
        // Teste com função de multiplicação
        {
            TabelaHashEncadeada tabela(tamanhoTabela);
            
            double tempoInsercao = medirTempo([&]() {
                for (int valor : dados) {
                    tabela.inserir(valor, TabelaHashEncadeada::TipoHash::MULTIPLICACAO);
                }
            });
            
            double tempoBusca = medirTempo([&]() {
                for (int valor : dadosBusca) {
                    tabela.buscar(valor, TabelaHashEncadeada::TipoHash::MULTIPLICACAO);
                }
            });
            
            ResultadoTeste resultado;
            resultado.tipoTabela = "Encadeada";
            resultado.tamanhoTabela = tamanhoTabela;
            resultado.quantidadeDados = dados.size();
            resultado.tipoFuncaoHash = "Multiplicacao";
            resultado.tempoInsercao = tempoInsercao;
            resultado.tempoBusca = tempoBusca;
            resultado.colisoes = contarColisoesEncadeada(tabela);
            resultado.fatorCarga = tabela.fatorCarga();
            
            resultados.push_back(resultado);
        }
        
        std::cout << " ✓" << std::endl;
    }
    
    /**
     * @brief Testa o desempenho de tabelas hash abertas
     * @param dados Vetor com dados para inserção
     * @param dadosBusca Vetor com dados para busca
     */
    void testarTabelaAberta(const std::vector<int>& dados,
                           const std::vector<int>& dadosBusca) {
        const size_t TAMANHO_TABELA_ABERTA = 50009; // Conforme especificação
        std::cout << "  Testando tabela aberta (tamanho: " << TAMANHO_TABELA_ABERTA << ")...";
        
        // Teste com função de divisão
        {
            TabelaHashAberta tabela(TAMANHO_TABELA_ABERTA);
            
            double tempoInsercao = medirTempo([&]() {
                for (int valor : dados) {
                    try {
                        tabela.inserir(valor, TabelaHashAberta::TipoHash::DIVISAO);
                    } catch (const std::runtime_error&) {
                        // Tabela cheia, continuar
                        break;
                    }
                }
            });
            
            double tempoBusca = medirTempo([&]() {
                for (int valor : dadosBusca) {
                    tabela.buscar(valor, TabelaHashAberta::TipoHash::DIVISAO);
                }
            });
            
            ResultadoTeste resultado;
            resultado.tipoTabela = "Aberta";
            resultado.tamanhoTabela = TAMANHO_TABELA_ABERTA;
            resultado.quantidadeDados = tabela.getNumElementos();
            resultado.tipoFuncaoHash = "Divisao";
            resultado.tempoInsercao = tempoInsercao;
            resultado.tempoBusca = tempoBusca;
            resultado.colisoes = contarColisoesAberta(tabela);
            resultado.fatorCarga = tabela.fatorCarga();
            
            resultados.push_back(resultado);
        }
        
        // Teste com função de multiplicação
        {
            TabelaHashAberta tabela(TAMANHO_TABELA_ABERTA);
            
            double tempoInsercao = medirTempo([&]() {
                for (int valor : dados) {
                    try {
                        tabela.inserir(valor, TabelaHashAberta::TipoHash::MULTIPLICACAO);
                    } catch (const std::runtime_error&) {
                        // Tabela cheia, continuar
                        break;
                    }
                }
            });
            
            double tempoBusca = medirTempo([&]() {
                for (int valor : dadosBusca) {
                    tabela.buscar(valor, TabelaHashAberta::TipoHash::MULTIPLICACAO);
                }
            });
            
            ResultadoTeste resultado;
            resultado.tipoTabela = "Aberta";
            resultado.tamanhoTabela = TAMANHO_TABELA_ABERTA;
            resultado.quantidadeDados = tabela.getNumElementos();
            resultado.tipoFuncaoHash = "Multiplicacao";
            resultado.tempoInsercao = tempoInsercao;
            resultado.tempoBusca = tempoBusca;
            resultado.colisoes = contarColisoesAberta(tabela);
            resultado.fatorCarga = tabela.fatorCarga();
            
            resultados.push_back(resultado);
        }
        
        std::cout << " ✓" << std::endl;
    }
    
    /**
     * @brief Salva os resultados em arquivo CSV
     * @param nomeArquivo Nome do arquivo de saída
     */
    void salvarResultados(const std::string& nomeArquivo) {
        std::ofstream arquivo(nomeArquivo);
        
        if (!arquivo.is_open()) {
            throw std::runtime_error("Erro ao criar arquivo: " + nomeArquivo);
        }
        
        // Cabeçalho CSV
        arquivo << "TipoTabela,TamanhoTabela,QuantidadeDados,FuncaoHash,"
                << "TempoInsercao(ms),TempoBusca(ms),Colisoes,FatorCarga\n";
        
        // Dados
        for (const auto& resultado : resultados) {
            arquivo << resultado.tipoTabela << ","
                   << resultado.tamanhoTabela << ","
                   << resultado.quantidadeDados << ","
                   << resultado.tipoFuncaoHash << ","
                   << std::fixed << std::setprecision(3)
                   << resultado.tempoInsercao << ","
                   << resultado.tempoBusca << ","
                   << resultado.colisoes << ","
                   << std::setprecision(4)
                   << resultado.fatorCarga << "\n";
        }
        
        arquivo.close();
        std::cout << "\n✓ Resultados salvos em: " << nomeArquivo << std::endl;
    }
    
    /**
     * @brief Imprime um relatório resumido dos testes
     */
    void imprimirRelatorio() {
        if (resultados.empty()) {
            std::cout << "Nenhum resultado disponível.\n";
            return;
        }
        
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "                        RELATÓRIO DE PERFORMANCE\n";
        std::cout << std::string(80, '=') << std::endl;
        
        // Cabeçalho
        std::cout << std::left
                 << std::setw(10) << "Tipo"
                 << std::setw(8) << "Tam.Tab"
                 << std::setw(8) << "Dados"
                 << std::setw(12) << "Hash"
                 << std::setw(12) << "Inser.(ms)"
                 << std::setw(12) << "Busca(ms)"
                 << std::setw(8) << "Colisões"
                 << std::setw(8) << "F.Carga"
                 << std::endl;
        
        std::cout << std::string(80, '-') << std::endl;
        
        // Dados
        for (const auto& resultado : resultados) {
            std::cout << std::left
                     << std::setw(10) << resultado.tipoTabela
                     << std::setw(8) << resultado.tamanhoTabela
                     << std::setw(8) << resultado.quantidadeDados
                     << std::setw(12) << resultado.tipoFuncaoHash
                     << std::setw(12) << std::fixed << std::setprecision(3) << resultado.tempoInsercao
                     << std::setw(12) << std::fixed << std::setprecision(3) << resultado.tempoBusca
                     << std::setw(8) << resultado.colisoes
                     << std::setw(8) << std::fixed << std::setprecision(4) << resultado.fatorCarga
                     << std::endl;
        }
        
        std::cout << std::string(80, '=') << std::endl;
    }
    
    /**
     * @brief Retorna os resultados dos testes
     * @return Vetor com todos os resultados
     */
    const std::vector<ResultadoTeste>& getResultados() const {
        return resultados;
    }
};

/**
 * @brief Função principal do programa
 */
int main(int argc, char* argv[]) {
    try {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "    ANÁLISE COMPARATIVA DE TABELAS HASH" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        GeradorDados gerador;
        BenchmarkManager benchmark;
        
        // Configurações conforme especificação do trabalho
        const std::vector<size_t> TAMANHOS_TABELA_ENCADEADA = {29, 97, 251, 499, 911};
        const std::vector<std::string> ARQUIVOS_DADOS = {
            "data/numeros_aleatorios_100.txt",
            "data/numeros_aleatorios_500.txt",
            "data/numeros_aleatorios_1000.txt",
            "data/numeros_aleatorios_5000.txt",
            "data/numeros_aleatorios_10000.txt",
            "data/numeros_aleatorios_50000.txt"
        };
        
        // Gerar dados de busca (1000 números aleatórios)
        std::cout << "\nGerando dados para busca (1000 números aleatórios)...";
        auto dadosBusca = gerador.gerarNumerosAleatoriosComRepeticao(1000);
        std::cout << " ✓\n";
        
        // Executar testes para cada arquivo de dados
        for (const std::string& arquivo : ARQUIVOS_DADOS) {
            try {
                std::cout << "\nCarregando dados de: " << arquivo << std::endl;
                auto dados = gerador.carregarDeArquivo(arquivo);
                
                std::cout << "Executando testes com " << dados.size() << " elementos:" << std::endl;
                
                // Testar tabelas encadeadas com diferentes tamanhos
                for (size_t tamanho : TAMANHOS_TABELA_ENCADEADA) {
                    benchmark.testarTabelaEncadeada(dados, dadosBusca, tamanho);
                }
                
                // Testar tabela aberta
                benchmark.testarTabelaAberta(dados, dadosBusca);
                
            } catch (const std::exception& e) {
                std::cerr << "Erro ao processar arquivo " << arquivo 
                         << ": " << e.what() << std::endl;
                continue;
            }
        }
        
        // Gerar relatórios
        benchmark.imprimirRelatorio();
        benchmark.salvarResultados("resultados_benchmark.csv");
        
        std::cout << "\n✅ Análise concluída com sucesso!\n" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Erro crítico: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Erro desconhecido ocorreu." << std::endl;
        return 1;
    }
}