#pragma once
#include <vector>
#include <string>
#include <random>

class GeradorDados {
private:
    std::mt19937 gerador;
    
public:
    explicit GeradorDados(unsigned int seed = std::random_device{}());
    
    std::vector<int> gerarNumerosAleatorios(size_t quantidade, 
                                           int minimo = 1, 
                                           int maximo = 1000000);
    
    bool salvarEmArquivo(const std::vector<int>& numeros, 
                        const std::string& nomeArquivo);
    
    std::vector<int> carregarDeArquivo(const std::string& nomeArquivo);
};