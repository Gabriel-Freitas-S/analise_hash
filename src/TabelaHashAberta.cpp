#include "TabelaHashAberta.hpp"

// Construtor: inicializa a tabela com um tamanho específico
TabelaHashAberta::TabelaHashAberta(int tamanhoTabela) : tamanho(tamanhoTabela) {
    tabela.resize(tamanho, -1); // Inicializa com -1 para indicar posições vazias
    posicoesOcupadas.resize(tamanho, false);
}

// Função de hash usando o método da divisão
int TabelaHashAberta::funcaoHashDivisao(int chave) {
    return chave % tamanho;
}

// Função de hash usando o método da multiplicação
int TabelaHashAberta::funcaoHashMultiplicacao(int chave) {
    const double c = 0.63274838;
    double val = chave * c;
    double parteFracionaria = val - static_cast<int>(val);
    return static_cast<int>(tamanho * parteFracionaria);
}

// Insere uma chave na tabela hash usando sondagem linear
void TabelaHashAberta::inserir(int chave, bool usarMultiplicacao) {
    int indice;
    if (usarMultiplicacao) {
        indice = funcaoHashMultiplicacao(chave);
    } else {
        indice = funcaoHashDivisao(chave);
    }

    // Sondagem linear para encontrar uma posição livre
    while (posicoesOcupadas[indice]) {
        indice = (indice + 1) % tamanho;
    }

    tabela[indice] = chave;
    posicoesOcupadas[indice] = true;
}

// Busca uma chave na tabela hash
bool TabelaHashAberta::buscar(int chave, bool usarMultiplicacao) {
    int indice;
    if (usarMultiplicacao) {
        indice = funcaoHashMultiplicacao(chave);
    } else {
        indice = funcaoHashDivisao(chave);
    }

    int indiceOriginal = indice;

    // Procura pela chave a partir do índice inicial
    while (posicoesOcupadas[indice]) {
        if (tabela[indice] == chave) {
            return true; // Chave encontrada
        }
        indice = (indice + 1) % tamanho;

        // Se voltou ao início, a chave não está na tabela
        if (indice == indiceOriginal) {
            break;
        }
    }

    return false; // Chave não encontrada
}

// Retorna o tamanho da tabela
int TabelaHashAberta::getTamanho() const {
    return tamanho;
}
