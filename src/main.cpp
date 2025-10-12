#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>

#include "TabelaHashEncadeada.hpp"
#include "TabelaHashAberta.hpp"
#include "GeradorDados.hpp"

// Função para medir o tempo de inserção e busca
void executarTeste(const std::string& tipoTabela, const std::string& tipoHash, int tamanhoTabela, const std::vector<int>& dadosInserir, const std::vector<int>& dadosBuscar) {
    
    auto inicioInsercao = std::chrono::high_resolution_clock::now();

    if (tipoTabela == "Encadeada") {
        TabelaHashEncadeada tabela(tamanhoTabela);
        for (int chave : dadosInserir) {
            tabela.inserir(chave, tipoHash == "Multiplicacao");
        }
        auto fimInsercao = std::chrono::high_resolution_clock::now();

        auto inicioBusca = std::chrono::high_resolution_clock::now();
        for (int chave : dadosBuscar) {
            tabela.buscar(chave, tipoHash == "Multiplicacao");
        }
        auto fimBusca = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> duracaoInsercao = fimInsercao - inicioInsercao;
        std::chrono::duration<double, std::milli> duracaoBusca = fimBusca - inicioBusca;

        std::cout << std::left << std::setw(12) << tipoTabela 
                  << std::setw(15) << tipoHash 
                  << std::setw(10) << tamanhoTabela 
                  << std::setw(15) << dadosInserir.size() 
                  << std::fixed << std::setprecision(4) << std::setw(15) << duracaoInsercao.count() 
                  << std::setw(15) << duracaoBusca.count() << std::endl;

    } else { // Endereçamento Aberto
        TabelaHashAberta tabela(tamanhoTabela);
        for (int chave : dadosInserir) {
            tabela.inserir(chave, tipoHash == "Multiplicacao");
        }
        auto fimInsercao = std::chrono::high_resolution_clock::now();

        auto inicioBusca = std::chrono::high_resolution_clock::now();
        for (int chave : dadosBuscar) {
            tabela.buscar(chave, tipoHash == "Multiplicacao");
        }
        auto fimBusca = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> duracaoInsercao = fimInsercao - inicioInsercao;
        std::chrono::duration<double, std::milli> duracaoBusca = fimBusca - inicioBusca;

        std::cout << std::left << std::setw(12) << tipoTabela 
                  << std::setw(15) << tipoHash 
                  << std::setw(10) << tamanhoTabela 
                  << std::setw(15) << dadosInserir.size() 
                  << std::fixed << std::setprecision(4) << std::setw(15) << duracaoInsercao.count() 
                  << std::setw(15) << duracaoBusca.count() << std::endl;
    }
}

int main() {
    // --- Geração dos Arquivos de Dados ---
    std::vector<int> tamanhosInserir = {100, 500, 1000, 5000, 10000, 50000};
    for (int tamanho : tamanhosInserir) {
        GeradorDados::gerarArquivoNumeros("data/numeros_" + std::to_string(tamanho) + ".txt", tamanho);
    }
    GeradorDados::gerarArquivoNumeros("data/numeros_busca.txt", 1000);

    // --- Carregamento dos Dados ---
    std::vector<int> dadosBusca = GeradorDados::carregarNumeros("data/numeros_busca.txt");

    // --- Execução dos Testes ---
    std::cout << "Iniciando análise comparativa de hashing...\n\n";
    std::cout << std::left << std::setw(12) << "Tabela" 
              << std::setw(15) << "Hash" 
              << std::setw(10) << "Tam. Tab." 
              << std::setw(15) << "N Inseridos" 
              << std::setw(15) << "T. Insercao(ms)" 
              << std::setw(15) << "T. Busca(ms)" << std::endl;
    std::cout << std::string(82, '-') << std::endl;

    // Testes para Tabela com Encadeamento
    std::vector<int> tamanhosTabelaEncadeada = {29, 97, 251, 499, 911};
    for (int n : tamanhosInserir) {
        std::vector<int> dadosInserir = GeradorDados::carregarNumeros("data/numeros_" + std::to_string(n) + ".txt");
        for (int tamanhoTabela : tamanhosTabelaEncadeada) {
            executarTeste("Encadeada", "Divisao", tamanhoTabela, dadosInserir, dadosBusca);
            executarTeste("Encadeada", "Multiplicacao", tamanhoTabela, dadosInserir, dadosBusca);
        }
    }

    std::cout << std::string(82, '-') << std::endl;

    // Testes para Tabela com Endereçamento Aberto
    const int tamanhoTabelaAberta = 50009;
    for (int n : tamanhosInserir) {
         if (n > tamanhoTabelaAberta) continue; // Não podemos inserir mais elementos que o tamanho da tabela
        std::vector<int> dadosInserir = GeradorDados::carregarNumeros("data/numeros_" + std::to_string(n) + ".txt");
        executarTeste("Aberta", "Divisao", tamanhoTabelaAberta, dadosInserir, dadosBusca);
        executarTeste("Aberta", "Multiplicacao", tamanhoTabelaAberta, dadosInserir, dadosBusca);
    }

    std::cout << "\nAnálise concluída." << std::endl;

    return 0;
}
