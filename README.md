# 📋 Pesquisa e Ordenação: Análise Comparativa de Soluções Baseadas em Hashing

[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.15%2B-green.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen.svg)](#)

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

## 🔧 Pré-requisitos

### 💻 Sistema Operacional
- **Windows** (Visual Studio 2017+, CLion, Code::Blocks)
- **Linux** (GCC 7+, Clang 6+)
- **macOS** (Xcode 10+, Homebrew GCC)

### 🔨 Ferramentas Necessárias
- **CMake** 3.15 ou superior
- **Compilador C++17** compatível:
  - GCC 7.0+
  - Clang 6.0+
  - MSVC 2017+

### 📦 Instalação de Dependências

#### Windows (com Chocolatey)
```cmd
choco install cmake
choco install visualstudio2022-workload-vctools
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install cmake g++ build-essential
```

#### Linux (CentOS/RHEL)
```bash
sudo yum install cmake gcc-c++ make
```

#### macOS
```bash
brew install cmake
# Instalar Xcode Command Line Tools
xcode-select --install
```

## 🚀 Como Executar

### 🏢 Método 1: CLion (Recomendado)

1. **Abrir o Projeto:**
   - Inicie o CLion
   - Selecione `File > Open`
   - Navegue até o diretório `analise_hash`
   - Aguarde a sincronização do CMake

2. **Compilar:**
   - Clique no ícone de martelo (🔨) na barra de ferramentas
   - Ou `Build > Build Project`

3. **Executar:**
   - Clique no botão "Play" (▶️)
   - Os resultados aparecerão no console

### 💻 Método 2: Linha de Comando

#### Windows (cmd/PowerShell)
```cmd
mkdir build
cd build
cmake ..
cmake --build . --config Release
.\bin\analise_hash.exe
```

#### Linux/macOS
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./bin/analise_hash
```

### 🛠️ Opções Avançadas de Build

```bash
# Build com otimizações máximas
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build para debug com sanitizers
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZERS=ON

# Build silencioso (sem warnings)
cmake .. -DENABLE_WARNINGS=OFF

# Limpar resultados anteriores
make clean_results

# Executar apenas benchmarks
make benchmark
```

## 📈 Saídas do Programa

### 📊 Arquivo CSV
O programa gera automaticamente `resultados_benchmark.csv` contendo:
- Tipo de tabela (Encadeada/Aberta)
- Tamanho da tabela
- Quantidade de dados inseridos
- Função hash utilizada
- Tempo de inserção (ms)
- Tempo de busca (ms)
- Número de colisões
- Fator de carga

### 📝 Relatório Console
Exibe uma tabela formatada com resumo de todos os testes:

```
================================================================================
                        RELATÓRIO DE PERFORMANCE
================================================================================
Tipo      Tam.Tab  Dados    Hash         Inser.(ms)   Busca(ms)    Colisões  F.Carga
--------------------------------------------------------------------------------
Encadeada 29       100      Divisao      0.234        0.156        15       3.4483
Encadeada 29       100      Multiplicacao 0.198        0.142        12       3.4483
...
```

## 🧪 Parâmetros de Teste

### 📏 Configurações das Tabelas

| Tipo | Tamanhos Testados | Observações |
|------|------------------|---------------|
| **Encadeada** | 29, 97, 251, 499, 911 | Números primos para melhor distribuição |
| **Aberta** | 50,009 | Tamanho fixo com sondagem linear |

### 📊 Datasets Utilizados

| Arquivo | Elementos | Descrição |
|---------|-----------|-------------|
| `numeros_aleatorios_100.txt` | 100 | Dataset pequeno |
| `numeros_aleatorios_500.txt` | 500 | Dataset pequeno-médio |
| `numeros_aleatorios_1000.txt` | 1,000 | Dataset médio |
| `numeros_aleatorios_5000.txt` | 5,000 | Dataset médio-grande |
| `numeros_aleatorios_10000.txt` | 10,000 | Dataset grande |
| `numeros_aleatorios_50000.txt` | 50,000 | Dataset muito grande |

### 🔎 Métricas Avaliadas

- **⏱️ Tempo de Inserção:** Tempo para inserir todos os elementos
- **🔍 Tempo de Busca:** Tempo para buscar 1000 elementos aleatórios
- **⚔️ Colisões:** Número estimado de colisões ocorridas
- **⚖️ Fator de Carga:** Razão elementos/tamanho da tabela
- **📊 Distribuição:** Análise de clustering (apenas tabela aberta)

## 🔬 Análise dos Resultados

### 📈 Interpretação das Métricas

1. **Tabelas Encadeadas:**
   - ✅ Melhor para alto fator de carga
   - ✅ Performance constante independente do fator de carga
   - ❌ Overhead de memória dos ponteiros
   - ❌ Cache miss mais frequente

2. **Tabelas Abertas:**
   - ✅ Melhor localidade de referência (cache-friendly)
   - ✅ Menor uso de memória
   - ❌ Performance degrada com alto fator de carga
   - ❌ Clustering pode ser problemático

### 📊 Expectativas de Performance

| Cenário | Encadeada | Aberta | Melhor Opção |
|----------|-----------|--------|--------------|
| Baixo fator de carga (<0.5) | Boa | Ótima | **Aberta** |
| Alto fator de carga (>0.75) | Boa | Ruim | **Encadeada** |
| Muitas inserções | Boa | Média | **Encadeada** |
| Muitas buscas | Média | Boa | **Aberta** |

## 🔧 Resolução de Problemas

### ❌ Erros Comuns

#### "CMake not found"
**Solução:** Instalar CMake e adicionar ao PATH do sistema

#### "Compiler not found"
**Windows:** Instalar Visual Studio Build Tools
**Linux:** `sudo apt install build-essential`

#### "Arquivo não encontrado: data/..."
**Solução:** Verificar se está executando no diretório raiz do projeto

#### Performance muito baixa
**Solução:** Compilar em modo Release (`-DCMAKE_BUILD_TYPE=Release`)

### 📝 Log de Debug

Para habilitar logs detalhados:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZERS=ON
```

## 🚀 Extensões e Melhorias

### 🎆 Features Implementadas
- ✅ Benchmarking automatizado
- ✅ Export para CSV
- ✅ Validação de arquivos
- ✅ Estatísticas detalhadas
- ✅ Tratamento de erros robusto
- ✅ Documentação completa
- ✅ Suporte multiplataforma

### 🔮 Possibilidades Futuras
- [ ] Outras funções de hash (SHA, MD5)
- [ ] Diferentes estratégias de sondagem (quadrática, duplo hashing)
- [ ] Interface gráfica para visualização
- [ ] Testes unitários automatizados
- [ ] Benchmark com datasets reais
- [ ] Análise estatística avançada

## 📚 Referências Acadêmicas

1. **Cormen, T. H.** et al. *Introduction to Algorithms*. 3rd ed. MIT Press, 2009.
2. **Knuth, D. E.** *The Art of Computer Programming, Volume 3: Sorting and Searching*. 2nd ed. Addison-Wesley, 1998.
3. **Sedgewick, R.** *Algorithms in C++*. 3rd ed. Addison-Wesley, 2002.

## 👥 Contribuição

Contribuições são bem-vindas! Por favor:

1. Fork o projeto
2. Crie uma branch para sua feature (`git checkout -b feature/AmazingFeature`)
3. Commit suas mudanças (`git commit -m 'Add some AmazingFeature'`)
4. Push para a branch (`git push origin feature/AmazingFeature`)
5. Abra um Pull Request

## 📜 Licença

Este projeto é distribuído sob a licença MIT. Veja `LICENSE` para mais informações.

---

**🎆 Desenvolvido com ❤️ para a disciplina de Pesquisa e Ordenação**

*📅 Última atualização: Outubro 2024*