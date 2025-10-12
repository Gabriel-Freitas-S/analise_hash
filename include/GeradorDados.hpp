#ifndef GERADOR_DADOS_HPP
#define GERADOR_DADOS_HPP

#include <vector>
#include <string>

class GeradorDados {
public:
    // Gera um arquivo com 'quantidade' de números aleatórios
    static void gerarArquivoNumeros(const std::string& nomeArquivo, int quantidade, int maxValor = 1000000);

    // Carrega os números de um arquivo para um vetor
    static std::vector<int> carregarNumeros(const std::string& nomeArquivo);
};

#endif // GERADOR_DADOS_HPP
