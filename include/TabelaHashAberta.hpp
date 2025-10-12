#ifndef TABELA_HASH_ABERTA_HPP
#define TABELA_HASH_ABERTA_HPP

#include <vector>

class TabelaHashAberta {
private:
    int tamanho;
    std::vector<int> tabela;
    std::vector<bool> posicoesOcupadas;

    // Funções de hash
    int funcaoHashDivisao(int chave);
    int funcaoHashMultiplicacao(int chave);

public:
    // Construtor
    TabelaHashAberta(int tamanhoTabela);

    // Operações
    void inserir(int chave, bool usarMultiplicacao = false);
    bool buscar(int chave, bool usarMultiplicacao = false);

    // Getters
    int getTamanho() const;
};

#endif // TABELA_HASH_ABERTA_HPP
