#ifndef TABELA_HASH_ENCADEADA_HPP
#define TABELA_HASH_ENCADEADA_HPP

#include <list>
#include <vector>

// Estrutura para representar um nó na lista encadeada
struct No {
    int chave;
    // Outros dados podem ser adicionados aqui
};

class TabelaHashEncadeada {
private:
    int tamanho;
    std::vector<std::list<No>> tabela;

    // Funções de hash
    int funcaoHashDivisao(int chave);
    int funcaoHashMultiplicacao(int chave);

public:
    // Construtor
    TabelaHashEncadeada(int tamanhoTabela);

    // Operações
    void inserir(int chave, bool usarMultiplicacao = false);
    bool buscar(int chave, bool usarMultiplicacao = false);

    // Getters
    int getTamanho() const;
};

#endif // TABELA_HASH_ENCADEADA_HPP
