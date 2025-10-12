#include "TabelaHashEncadeada.hpp"

// Construtor: inicializa a tabela com um tamanho específico
TabelaHashEncadeada::TabelaHashEncadeada(int tamanhoTabela) : tamanho(tamanhoTabela) {
    tabela.resize(tamanho);
}

// Função de hash usando o método da divisão
int TabelaHashEncadeada::funcaoHashDivisao(int chave) {
    return chave % tamanho;
}

// Função de hash usando o método da multiplicação
int TabelaHashEncadeada::funcaoHashMultiplicacao(int chave) {
    const double c = 0.63274838;
    double val = chave * c;
    double parteFracionaria = val - static_cast<int>(val);
    return static_cast<int>(tamanho * parteFracionaria);
}

// Insere uma chave na tabela hash
void TabelaHashEncadeada::inserir(int chave, bool usarMultiplicacao) {
    int indice;
    if (usarMultiplicacao) {
        indice = funcaoHashMultiplicacao(chave);
    } else {
        indice = funcaoHashDivisao(chave);
    }
    tabela[indice].push_back({chave});
}

// Busca uma chave na tabela hash
bool TabelaHashEncadeada::buscar(int chave, bool usarMultiplicacao) {
    int indice;
    if (usarMultiplicacao) {
        indice = funcaoHashMultiplicacao(chave);
    } else {
        indice = funcaoHashDivisao(chave);
    }

    // Percorre a lista encadeada no índice correspondente
    for (const auto& no : tabela[indice]) {
        if (no.chave == chave) {
            return true; // Chave encontrada
        }
    }
    return false; // Chave não encontrada
}

// Retorna o tamanho da tabela
int TabelaHashEncadeada::getTamanho() const {
    return tamanho;
}
