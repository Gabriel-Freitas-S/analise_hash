#include "TabelaHashAberta.hpp"
#include <cmath>

TabelaHashAberta::TabelaHashAberta(size_t tam) 
    : tamanho(tam), numElementos(0) {
    if (tam == 0) {
        throw std::invalid_argument("Tamanho da tabela deve ser maior que zero");
    }
    tabela.resize(tamanho);
}

size_t TabelaHashAberta::calcularHashDivisao(int chave) const {
    return static_cast<size_t>(std::abs(chave)) % tamanho;
}

size_t TabelaHashAberta::calcularHashMultiplicacao(int chave) const {
    double produto = std::abs(chave) * CONSTANTE_MULTIPLICACAO;
    double fracao = produto - std::floor(produto);
    return static_cast<size_t>(std::floor(fracao * tamanho));
}

size_t TabelaHashAberta::sondagemLinear(size_t indiceInicial, int valor) const {
    size_t tentativas = 0;
    size_t indice = indiceInicial;
    
    while (tentativas < tamanho) {
        if (tabela[indice].estado == Celula::Estado::VAZIO || 
            tabela[indice].estado == Celula::Estado::REMOVIDO ||
            (tabela[indice].estado == Celula::Estado::OCUPADO && 
             tabela[indice].valor == valor)) {
            return indice;
        }
        indice = (indice + 1) % tamanho;
        ++tentativas;
    }
    
    throw std::runtime_error("Tabela cheia - não foi possível inserir");
}

void TabelaHashAberta::inserir(int valor, TipoHash tipo) {
    if (fatorCarga() >= MAX_FATOR_CARGA) {
        throw std::runtime_error("Fator de carga muito alto");
    }
    
    size_t indiceInicial = (tipo == TipoHash::DIVISAO) 
        ? calcularHashDivisao(valor) 
        : calcularHashMultiplicacao(valor);
    
    size_t indice = sondagemLinear(indiceInicial, valor);
    
    // Não inserir duplicatas
    if (tabela[indice].estado == Celula::Estado::OCUPADO && 
        tabela[indice].valor == valor) {
        return;
    }
    
    if (tabela[indice].estado != Celula::Estado::OCUPADO) {
        tabela[indice] = Celula(valor);
        ++numElementos;
    }
}

bool TabelaHashAberta::buscar(int valor, TipoHash tipo) const {
    size_t indiceInicial = (tipo == TipoHash::DIVISAO) 
        ? calcularHashDivisao(valor) 
        : calcularHashMultiplicacao(valor);
    
    size_t tentativas = 0;
    size_t indice = indiceInicial;
    
    while (tentativas < tamanho && 
           tabela[indice].estado != Celula::Estado::VAZIO) {
        if (tabela[indice].estado == Celula::Estado::OCUPADO && 
            tabela[indice].valor == valor) {
            return true;
        }
        indice = (indice + 1) % tamanho;
        ++tentativas;
    }
    
    return false;
}