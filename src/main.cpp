#include <iostream>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <vector>
#include "TabelaHashEncadeada.hpp"
#include "TabelaHashAberta.hpp"
#include "GeradorDados.hpp"

struct ResultadoTeste {
    std::string tipo;
    size_t tamanho;
    std::string funcaoHash;
    double tempoInsercao;
    double tempoBusca;
    size_t colisoes;
};

class Benchmark {
private:
    std::vector<ResultadoTeste> resultados;
    
public:
    template<typename Func>
    double medirTempo(Func&& func) {
        auto inicio = std::chrono::high_resolution_clock::now();
        func();
        auto fim = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duracao = fim - inicio;
        return duracao.count();
    }
    
    void testarTabelaEncadeada(const std::vector<int>& dados,
                              const std::vector<int>& buscas,
                              size_t tamanho);
    
    void testarTabelaAberta(const std::vector<int>& dados,
                           const std::vector<int>& buscas);
    
    void salvarResultados(const std::string& arquivo);
    void imprimirRelatorio();
};

int main(int argc, char* argv[]) {
    try {
        GeradorDados gerador;
        Benchmark benchmark;
        
        // Tamanhos de teste conforme especificação
        std::vector<size_t> tamanhosTabela = {29, 97, 251, 499, 911};
        std::vector<size_t> tamanhosDados = {100, 500, 1000, 5000, 10000, 50000};
        
        // Gerar dados de busca
        auto dadosBusca = gerador.gerarNumerosAleatorios(1000, 1, 1000000);
        
        for (size_t tamDados : tamanhosDados) {
            std::cout << "
=== Testando com " << tamDados << " elementos ===
";
            
            auto dados = gerador.gerarNumerosAleatorios(tamDados, 1, 1000000);
            
            // Testar tabelas encadeadas
            for (size_t tamTabela : tamanhosTabela) {
                benchmark.testarTabelaEncadeada(dados, dadosBusca, tamTabela);
            }
            
            // Testar tabela aberta
            benchmark.testarTabelaAberta(dados, dadosBusca);
        }
        
        benchmark.imprimirRelatorio();
        benchmark.salvarResultados("resultados.csv");
        
        std::cout << "
Testes concluídos com sucesso!
";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
        return 1;
    }
}