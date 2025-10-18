# 📋 Pesquisa e Ordenação: Análise Comparativa de Soluções Baseadas em Hashing

[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.15%2B-green.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen.svg)](#)

## 👤 Autores

- Gabriel Freitas Souza ([GitHub](https://github.com/Gabriel-Freitas-S))
- Roberli Schuina Silva ([GitHub](https://github.com/RoberliSchuina))

Trabalho desenvolvido em dupla para a disciplina de Pesquisa e Ordenação.

## 🎯 Visão Geral

Este projeto, desenvolvido para a disciplina de **Pesquisa e Ordenação**, implementa e analisa comparativamente diferentes soluções de hashing. O objetivo é explorar mecanismos distintos para tratar colisões em tabelas hash, avaliando o desempenho de cada abordagem em cenários variados.

### 🔍 Abordagens de Tratamento de Colisão

- **🔗 Listas Encadeadas (Chaining):** Cada posição da tabela armazena uma lista encadeada com as chaves que colidiram
- **🏃 Endereçamento Aberto (Open Addressing):** Em caso de colisão, a chave é armazenada na próxima posição disponível usando sondagem linear

### 🧮 Funções de Hashing Implementadas

1. **Método da Divisão:**
   ```
   h(k) = k mod p
   ```
   onde `p` é o tamanho da tabela (preferencialmente primo)

2. **Método da Multiplicação:**
   ```
   h(k) = ⌊tr((k × c) mod 1) × p⌋
   ```
   onde `c = 0.6180339887` (fração áurea)

## 📜 Estrutura do Projeto

```
analise_hash/
│
├── 📁 include/              # Arquivos de cabeçalho
│   ├── TabelaHashEncadeada.hpp
│   ├── TabelaHashAberta.hpp
│   └── GeradorDados.hpp
│
├── 📂 src/                 # Código fonte
│   ├── main.cpp
│   ├── TabelaHashEncadeada.cpp
│   ├── TabelaHashAberta.cpp
│   └── GeradorDados.cpp
│
├── 📀 data/                # Datasets de teste
│   ├── numeros_aleatorios_100.txt
│   ├── numeros_aleatorios_500.txt
│   ├── numeros_aleatorios_1000.txt
│   ├── numeros_aleatorios_5000.txt
│   ├── numeros_aleatorios_10000.txt
│   └── numeros_aleatorios_50000.txt
│
├── CMakeLists.txt         # Configuração de build
├── README.md             # Este arquivo
└── .gitignore           # Arquivos ignorados pelo Git
```

### Arquivo CSV
O programa gera automaticamente `resultados_benchmark.csv` contendo:
- Tipo de tabela (Encadeada/Aberta)
- Tamanho da tabela
- Quantidade de dados inseridos
- Função hash utilizada
- Tempo de inserção (ms)
- Tempo de busca (ms)
- Número de colisões
- Fator de carga

### Relatório Console
Exibe uma tabela formatada com resumo de todos os testes.

## 🧪 Parâmetros de Teste

### Configurações das Tabelas

| Tipo | Tamanhos Testados | Observações |
|------|------------------|---------------|
| Encadeada | 29, 97, 251, 499, 911 | Números primos para melhor distribuição |
| Aberta | 50,009 | Tamanho fixo com sondagem linear |

### Datasets Utilizados

| Arquivo | Elementos | Descrição |
|---------|-----------|-------------|
| `numeros_aleatorios_100.txt` | 100 | Dataset pequeno |
| `numeros_aleatorios_500.txt` | 500 | Dataset pequeno-médio |
| `numeros_aleatorios_1000.txt` | 1,000 | Dataset médio |
| `numeros_aleatorios_5000.txt` | 5,000 | Dataset médio-grande |
| `numeros_aleatorios_10000.txt` | 10,000 | Dataset grande |
| `numeros_aleatorios_50000.txt` | 50,000 | Dataset muito grande |

### Métricas Avaliadas

- Tempo de Inserção
- Tempo de Busca
- Colisões estimadas
- Fator de Carga
- Distribuição/Clustering (apenas tabela aberta)

## Licença

Este projeto é distribuído sob a licença MIT.
