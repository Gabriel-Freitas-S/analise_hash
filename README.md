# Análise Comparativa de Soluções Baseadas em Hashing

Este projeto, desenvolvido em C++, implementa e analisa o desempenho de diferentes soluções de hashing para a disciplina de Pesquisa e Ordenação.

## Estruturas Implementadas

O projeto explora duas principais abordagens para tratamento de colisões em tabelas hash:

1.  **Listas Encadeadas**: Cada posição da tabela aponta para uma lista de chaves que colidiram naquele índice.
2.  **Endereçamento Aberto**: Em caso de colisão, a chave é inserida em uma posição alternativa na própria tabela (sondagem linear).

Para cada uma dessas abordagens, são utilizadas duas funções de hashing:

1.  **Método da Divisão**: `h(k) = k mod p`
2.  **Método da Multiplicação**: `h(k) = floor((k * c mod 1) * p)`

## Estrutura do Projeto

```
/
|-- CMakeLists.txt
|-- README.md
|-- .gitignore
|-- data/
|   |-- numeros_100.txt
|   |-- ... (outros arquivos de dados)
|-- doc/
|   |-- relatorio.pdf
|-- include/
|   |-- TabelaHashAberta.hpp
|   |-- TabelaHashEncadeada.hpp
|   |-- GeradorDados.hpp
|-- src/
|   |-- main.cpp
|   |-- TabelaHashAberta.cpp
|   |-- TabelaHashEncadeada.cpp
|   |-- GeradorDados.cpp
```

## Como Compilar e Executar

O projeto utiliza CMake para gerenciar a compilação.

1.  **Certifique-se de ter um compilador C++ e o CMake instalados.**
2.  **Crie um diretório de build:**
    ```bash
    mkdir build
    cd build
    ```
3.  **Execute o CMake e compile o projeto:**
    ```bash
    cmake ..
    make
    ```
4.  **Execute o programa:**
    ```bash
    ./analise_hash
    ```
