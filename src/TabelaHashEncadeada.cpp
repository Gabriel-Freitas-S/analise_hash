#include "TabelaHashEncadeada.hpp"
#include <cmath>

TabelaHashEncadeada::TabelaHashEncadeada(size_t tam) 
    : tamanho(tam), numElementos(0) {
    if (tam == 0) {
        throw std::invalid_argument("Tamanho da tabela deve ser maior que zero");
    }
    tabela.resize(tamanho);
}

size_t TabelaHashEncadeada::calcularHashDivisao(int chave) const {
    return static_cast<size_t>(std::abs(chave)) % tamanho;
}

size_t TabelaHashEncadeada::calcularHashMultiplicacao(int chave) const {
    double produto = std::abs(chave) * CONSTANTE_MULTIPLICACAO;
    double fracao = produto - std::floor(produto);
    return static_cast<size_t>(std::floor(fracao * tamanho));
}

void TabelaHashEncadeada::inserir(int valor, TipoHash tipo) {
    size_t indice = (tipo == TipoHash::DIVISAO) 
        ? calcularHashDivisao(valor) 
        : calcularHashMultiplicacao(valor);
    
    // Verificar se já existe
    if (buscar(valor, tipo)) {
        return; // Não inserir duplicatas
    }
    
    auto novoNo = std::make_unique<No>(valor);
    novoNo->proximo = std::move(tabela[indice]);
    tabela[indice] = std::move(novoNo);
    ++numElementos;
}

bool TabelaHashEncadeada::buscar(int valor, TipoHash tipo) const {
    size_t indice = (tipo == TipoHash::DIVISAO) 
        ? calcularHashDivisao(valor) 
        : calcularHashMultiplicacao(valor);
    
    const No* atual = tabela[indice].get();
    while (atual != nullptr) {
        if (atual->valor == valor) {
            return true;
        }
        atual = atual->proximo.get();
    }
    return false;
}