#include "GeradorDados.hpp"
#include <fstream>
#include <random>
#include <algorithm>

// Gera um arquivo com números aleatórios
void GeradorDados::gerarArquivoNumeros(const std::string& nomeArquivo, int quantidade, int maxValor) {
    std::ofstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        return; // Falha ao abrir o arquivo
    }

    arquivo << quantidade << "\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, maxValor);

    for (int i = 0; i < quantidade; ++i) {
        arquivo << distrib(gen) << "\n";
    }

    arquivo.close();
}

// Carrega os números de um arquivo para um vetor
std::vector<int> GeradorDados::carregarNumeros(const std::string& nomeArquivo) {
    std::ifstream arquivo(nomeArquivo);
    std::vector<int> numeros;
    int quantidade, valor;

    if (arquivo.is_open()) {
        arquivo >> quantidade;
        numeros.reserve(quantidade);
        while (arquivo >> valor) {
            numeros.push_back(valor);
        }
        arquivo.close();
    }

    return numeros;
}
