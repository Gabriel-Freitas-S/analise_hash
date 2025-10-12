# Pesquisa e Ordenação: Análise Comparativa de Soluções Baseadas em Hashing

## Visão Geral

Este trabalho, desenvolvido para a disciplina de Pesquisa e Ordenação, foca na implementação e análise comparativa de diferentes soluções de hashing. O objetivo é explorar mecanismos distintos para tratar colisões em tabelas hash, avaliando o desempenho de cada abordagem em cenários variados.

### Abordagens de Tratamento de Colisão

- **Listas Encadeadas:** Cada posição da tabela armazena uma lista encadeada com as chaves que colidiram.
- **Endereçamento Aberto:** Em caso de colisão, a chave é armazenada em uma posição alternativa (sondagem linear).

### Funções de Hashing

- **Método da Divisão:** `h(k) = k mod p`, onde `p` é o tamanho da tabela.
- **Método da Multiplicação:** `h(k) = floor((k * c mod 1) * p)`, com `c = 0.63274838`.

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

## Como Executar no Windows com CLion

Siga os passos abaixo para compilar e executar o projeto utilizando o CLion em um ambiente Windows.

### Pré-requisitos

**CLion IDE:** Certifique-se de que a versão mais recente do [CLion](https://www.jetbrains.com/clion/) está instalada.

### Passos para Execução

1.  **Abra o Projeto no CLion:**
    - Inicie o CLion.
    - Selecione **File > Open** e navegue até o diretório raiz do projeto `analise_hash`.
    - Aguarde o CLion carregar o projeto e sincronizar o CMake.

2.  **Compile o Projeto:**
    - O CLion geralmente compila o projeto automaticamente após carregá-lo.
    - Para compilar manualmente, clique no ícone de martelo ("Build") na barra de ferramentas ou vá para **Build > Build Project**.

3.  **Execute o Programa:**
    - Na barra de ferramentas superior, a configuração de execução `analise_hash` já deve estar selecionada.
    - Clique no ícone de "Play" (executar) ao lado dela para rodar o programa.
    - O resultado da execução, incluindo os tempos medidos, será exibido no console na parte inferior da IDE.