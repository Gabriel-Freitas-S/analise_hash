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

struct ResultadoTeste {
    std::string tipoTabela;
    size_t tamanhoTabela;
    size_t quantidadeDados;
    std::string tipoFuncaoHash;
    double tempoInsercao;
    double tempoBusca;
    size_t colisoes;
    double fatorCarga;
};

class BenchmarkManager {
private:
    std::vector<ResultadoTeste> resultados;

    template<typename Func>
    double medirTempo(Func&& func) {
        auto inicio = std::chrono::high_resolution_clock::now();
        func();
        auto fim = std::chrono::high_resolution_clock::now();
        auto duracao = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio);
        return duracao.count() / 1000.0;
    }

    size_t contarColisoesEncadeada(const TabelaHashEncadeada& tabela) {
        size_t elementos = tabela.getNumElementos();
        size_t tamanho = tabela.getTamanho();
        if (elementos <= tamanho) return 0;
        double lambda = static_cast<double>(elementos) / tamanho;
        return static_cast<size_t>(elementos - tamanho * (1 - std::exp(-lambda)));
    }

    size_t contarColisoesAberta(const TabelaHashAberta& tabela) {
        size_t elementos = tabela.getNumElementos();
        size_t tamanho = tabela.getTamanho();
        if (elementos <= tamanho) {
            double fc = tabela.fatorCarga();
            return static_cast<size_t>(elementos * fc / 2.0);
        }
        return elementos;
    }

public:
    void testarTabelaEncadeada(const std::vector<int>& dados,
                              const std::vector<int>& dadosBusca,
                              size_t tamanhoTabela) {
        std::cout << "  Testando tabela encadeada (tamanho: " << tamanhoTabela << ")...";

        {
            TabelaHashEncadeada tabela(tamanhoTabela);
            double tempoInsercao = medirTempo([&]() { for (int v : dados) tabela.inserir(v, TabelaHashEncadeada::TipoHash::DIVISAO); });
            double tempoBusca = medirTempo([&]() { for (int v : dadosBusca) tabela.buscar(v, TabelaHashEncadeada::TipoHash::DIVISAO); });
            resultados.push_back({"Encadeada", tamanhoTabela, dados.size(), "Divisao", tempoInsercao, tempoBusca, contarColisoesEncadeada(tabela), tabela.fatorCarga()});
        }
        {
            TabelaHashEncadeada tabela(tamanhoTabela);
            double tempoInsercao = medirTempo([&]() { for (int v : dados) tabela.inserir(v, TabelaHashEncadeada::TipoHash::MULTIPLICACAO); });
            double tempoBusca = medirTempo([&]() { for (int v : dadosBusca) tabela.buscar(v, TabelaHashEncadeada::TipoHash::MULTIPLICACAO); });
            resultados.push_back({"Encadeada", tamanhoTabela, dados.size(), "Multiplicacao", tempoInsercao, tempoBusca, contarColisoesEncadeada(tabela), tabela.fatorCarga()});
        }
        std::cout << " OK" << std::endl;
    }

    void testarTabelaAberta(const std::vector<int>& dados,
                           const std::vector<int>& dadosBusca) {
        const size_t TAM = 50009;
        std::cout << "  Testando tabela aberta (tamanho: " << TAM << ")...";
        {
            TabelaHashAberta tabela(TAM);
            double tempoInsercao = medirTempo([&]() {
                for (int v : dados) {
                    try { tabela.inserir(v, TabelaHashAberta::TipoHash::DIVISAO); } catch (...) { break; }
                }
            });
            double tempoBusca = medirTempo([&]() { for (int v : dadosBusca) tabela.buscar(v, TabelaHashAberta::TipoHash::DIVISAO); });
            resultados.push_back({"Aberta", TAM, tabela.getNumElementos(), "Divisao", tempoInsercao, tempoBusca, contarColisoesAberta(tabela), tabela.fatorCarga()});
        }
        {
            TabelaHashAberta tabela(TAM);
            double tempoInsercao = medirTempo([&]() {
                for (int v : dados) {
                    try { tabela.inserir(v, TabelaHashAberta::TipoHash::MULTIPLICACAO); } catch (...) { break; }
                }
            });
            double tempoBusca = medirTempo([&]() { for (int v : dadosBusca) tabela.buscar(v, TabelaHashAberta::TipoHash::MULTIPLICACAO); });
            resultados.push_back({"Aberta", TAM, tabela.getNumElementos(), "Multiplicacao", tempoInsercao, tempoBusca, contarColisoesAberta(tabela), tabela.fatorCarga()});
        }
        std::cout << " OK" << std::endl;
    }

    void salvarResultados(const std::string& arquivo) {
        std::ofstream arq(arquivo);
        if (!arq.is_open()) throw std::runtime_error("Erro ao criar arquivo: " + arquivo);
        arq << "TipoTabela,TamanhoTabela,QuantidadeDados,FuncaoHash,TempoInsercao(ms),TempoBusca(ms),Colisoes,FatorCarga\n";
        for (const auto& r : resultados) {
            arq << r.tipoTabela << "," << r.tamanhoTabela << "," << r.quantidadeDados << "," << r.tipoFuncaoHash << ","
                << std::fixed << std::setprecision(3) << r.tempoInsercao << "," << r.tempoBusca << "," << r.colisoes << ","
                << std::setprecision(4) << r.fatorCarga << "\n";
        }
        arq.close();
        std::cout << "\nResultados salvos em: " << arquivo << std::endl;
    }

    void imprimirRelatorio() {
        if (resultados.empty()) { std::cout << "Nenhum resultado disponivel." << std::endl; return; }
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "RELATORIO DE PERFORMANCE" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        std::cout << std::left
                  << std::setw(10) << "Tipo"
                  << std::setw(8)  << "Tam.Tab"
                  << std::setw(8)  << "Dados"
                  << std::setw(12) << "Hash"
                  << std::setw(12) << "Inser.(ms)"
                  << std::setw(12) << "Busca(ms)"
                  << std::setw(8)  << "Colisoes"
                  << std::setw(8)  << "F.Carga" << std::endl;
        std::cout << std::string(80, '-') << std::endl;
        for (const auto& r : resultados) {
            std::cout << std::left
                      << std::setw(10) << r.tipoTabela
                      << std::setw(8)  << r.tamanhoTabela
                      << std::setw(8)  << r.quantidadeDados
                      << std::setw(12) << r.tipoFuncaoHash
                      << std::setw(12) << std::fixed << std::setprecision(3) << r.tempoInsercao
                      << std::setw(12) << std::fixed << std::setprecision(3) << r.tempoBusca
                      << std::setw(8)  << r.colisoes
                      << std::setw(8)  << std::fixed << std::setprecision(4) << r.fatorCarga
                      << std::endl;
        }
        std::cout << std::string(80, '=') << std::endl;
    }
};

int main(int /*argc*/, char* /*argv*/[]) {
    try {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "ANALISE COMPARATIVA DE TABELAS HASH" << std::endl;
        std::cout << std::string(60, '=') << std::endl;

        GeradorDados gerador;
        BenchmarkManager benchmark;

        const std::vector<size_t> TAM_TABELA_ENCADEADA = {29, 97, 251, 499, 911};
        const std::vector<std::string> ARQS = {
            "data/numeros_aleatorios_100.txt",
            "data/numeros_aleatorios_500.txt",
            "data/numeros_aleatorios_1000.txt",
            "data/numeros_aleatorios_5000.txt",
            "data/numeros_aleatorios_10000.txt",
            "data/numeros_aleatorios_50000.txt"
        };

        std::cout << "\nGerando dados para busca (1000 numeros aleatorios)...";
        auto dadosBusca = gerador.gerarNumerosAleatoriosComRepeticao(1000);
        std::cout << " OK\n";

        for (const std::string& arquivo : ARQS) {
            try {
                std::cout << "\nCarregando dados de: " << arquivo << std::endl;
                auto dados = gerador.carregarDeArquivo(arquivo);
                std::cout << "Executando testes com " << dados.size() << " elementos:" << std::endl;
                for (size_t tamanho : TAM_TABELA_ENCADEADA) {
                    benchmark.testarTabelaEncadeada(dados, dadosBusca, tamanho);
                }
                benchmark.testarTabelaAberta(dados, dadosBusca);
            } catch (const std::exception& e) {
                std::cerr << "Erro ao processar arquivo " << arquivo << ": " << e.what() << std::endl;
                continue;
            }
        }

        benchmark.imprimirRelatorio();
        benchmark.salvarResultados("resultados_benchmark.csv");

        std::cout << "\nAnalise concluida com sucesso!\n" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Erro critico: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Erro desconhecido ocorreu." << std::endl;
        return 1;
    }
}
