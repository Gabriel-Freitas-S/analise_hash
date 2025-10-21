# 📋 Pesquisa e Ordenação: Análise Comparativa de Soluções Baseadas em Hashing

## 👤 Autores

- Gabriel Freitas Souza (GitHub)
- Roberli Schuina Silva (GitHub)

Trabalho desenvolvido em dupla para a disciplina de Pesquisa e Ordenação.

## 🔗 Links Importantes

- 📀 **[Página de Análise Completa](https://gabriel-freitas-s.github.io/analise_hash/)** - Visualize gráficos interativos e análise detalhada
- 📈 **Resultados CSV** - Dados brutos dos benchmarks
- 📁 **Código Fonte** - Implementações das tabelas hash

## 🎯 Visão Geral

Este projeto, desenvolvido para a disciplina de **Pesquisa e Ordenação**, implementa e analisa comparativamente diferentes soluções de hashing. O objetivo é explorar mecanismos distintos para tratar colisões em tabelas hash, avaliando o desempenho de cada abordagem em cenários variados.

### 🔍 Abordagens de Tratamento de Colisão

- **🔗 Encadeamento (Chaining):** Cada posição da tabela armazena uma lista encadeada com as chaves que colidiram
- **🏃 Endereçamento Aberto (Open Addressing):** Em caso de colisão, a chave é armazenada na próxima posição disponível usando sondagem linear

### 🧮 Funções de Hashing Implementadas

1. **Método da Divisão:**
   ```
   h(k) = k mod p
   ```
   onde `p` é o tamanho da tabela (preferencialmente primo)
   
2. **Método da Multiplicação:**
   ```
   h(k) = ⌊((k × c) mod 1) × p⌋
   ```
   onde `c = 0.63274838` (conforme especificação do trabalho)

## 📜 Estrutura do Projeto

```
analise_hash/
│
├── 📁 include/                    # Arquivos de cabeçalho (.hpp)
│   ├── TabelaEncadeada.hpp        # Interface da tabela com encadeamento
│   ├── TabelaAberta.hpp           # Interface da tabela com endereçamento aberto
│   └── CarregadorDados.hpp        # Interface do carregador de datasets
│
├── 📂 src/                        # Implementações (.cpp)
│   ├── main.cpp                   # Programa principal e benchmarks
│   ├── TabelaEncadeada.cpp        # Implementação do encadeamento
│   ├── TabelaAberta.cpp           # Implementação do endereçamento aberto
│   └── CarregadorDados.cpp        # Implementação do carregador
│
├── 📀 data/                       # Datasets de teste
│   ├── numeros_aleatorios_100.txt     # 100 números aleatórios
│   ├── numeros_aleatorios_500.txt     # 500 números aleatórios
│   ├── numeros_aleatorios_1000.txt    # 1.000 números aleatórios
│   ├── numeros_aleatorios_5000.txt    # 5.000 números aleatórios
│   ├── numeros_aleatorios_10000.txt   # 10.000 números aleatórios
│   └── numeros_aleatorios_50000.txt   # 50.000 números aleatórios
│
├── 📀 resultados_benchmark.csv    # Resultados dos testes (gerado automaticamente)
├── 📄 index.html                  # Página web com análise completa
├── ⚙️ CMakeLists.txt              # Configuração de build
├── 📋 README.md                   # Este arquivo
└── 🚫 .gitignore                 # Arquivos ignorados pelo Git
```

## 🛠️ Compilação e Execução

### Pré-requisitos

- **C++17** ou superior
- **CMake 3.15+**
- Compilador compatível (GCC, Clang, MSVC)

### Compilação

```bash
# Clone o repositório
git clone https://github.com/Gabriel-Freitas-S/analise_hash.git
cd analise_hash

# Crie o diretório de build
mkdir build && cd build

# Configure com CMake
cmake ..

# Compile
cmake --build .
```

### Execução

```bash
# Execute o programa principal
./analise_hash

# O programa irá:
# 1. Carregar os datasets da pasta data/
# 2. Executar benchmarks em todas as configurações
# 3. Gerar resultados_benchmark.csv
# 4. Exibir relatório no console
```

## 📀 Resultados e Análise

### Arquivo CSV Gerado

O programa gera automaticamente `resultados_benchmark.csv` com as seguintes colunas:

- **TipoTabela:** Encadeada ou Aberta
- **TamanhoTabela:** Tamanho da tabela utilizada
- **QuantidadeDados:** Número de elementos inseridos
- **FuncaoHash:** Divisao ou Multiplicacao
- **TempoInsercao(ms):** Tempo de inserção em milissegundos
- **TempoBusca(ms):** Tempo de busca em milissegundos
- **Colisoes:** Número estimado de colisões
- **FatorCarga:** Fator de carga da tabela

### Visualização Interativa

Acesse a **[Página de Análise Completa](https://gabriel-freitas-s.github.io/analise_hash/)** para:

- 📈 Gráficos interativos dos resultados (Chart.js)
- 📝 Análise detalhada do código
- 🔍 Discussão dos resultados
- 💡 Conclusões e recomendações

## 🧪 Parâmetros de Teste

### Configurações das Tabelas

| Tipo | Tamanhos Testados | Observações |
|------|-------------------|-------------|
| **Encadeada** | 29, 97, 251, 499, 911 | Números primos para melhor distribuição |
| **Aberta** | 50,009 | Tamanho fixo com sondagem linear |

### Datasets Utilizados

| Arquivo | Elementos | Descrição |
|---------|-----------|----------|
| `numeros_aleatorios_100.txt` | 100 | Dataset pequeno |
| `numeros_aleatorios_500.txt` | 500 | Dataset pequeno-médio |
| `numeros_aleatorios_1000.txt` | 1,000 | Dataset médio |
| `numeros_aleatorios_5000.txt` | 5,000 | Dataset médio-grande |
| `numeros_aleatorios_10000.txt` | 10,000 | Dataset grande |
| `numeros_aleatorios_50000.txt` | 50,000 | Dataset muito grande |

### Métricas Avaliadas

- ⏱️ **Tempo de Inserção:** Medido com `std::chrono::high_resolution_clock`
- 🔍 **Tempo de Busca:** Tempo para encontrar elementos na tabela
- 💥 **Colisões:** Número estimado de colisões durante inserções
- 📀 **Fator de Carga:** Razão entre elementos inseridos e tamanho da tabela
- 🧮 **Clustering:** Análise de agrupamento (apenas endereçamento aberto)

## 🎓 Conceitos Implementados

### Tabela Hash com Encadeamento (`TabelaEncadeada`)

- Lista encadeada em cada posição da tabela
- Inserção no início da lista (O(1))
- Busca sequencial na lista (O(n) no pior caso)
- Sem limitação de elementos

### Tabela Hash com Endereçamento Aberto (`TabelaAberta`)

- Sondagem linear para resolução de colisões
- Lazy deletion para remoções
- Controle automático de fator de carga
- Análise de clustering primário

### Carregador de Dados (`CarregadorDados`)

- Carregamento de datasets da pasta `data/`
- Geração de números aleatórios para testes
- Validação de integridade dos arquivos
- Análise estatística dos datasets

### Funções Hash

- **Divisão:** Simples e rápida, boa para tamanhos primos
- **Multiplicação:** Melhor distribuição, independente do tamanho

## 🏆 Principais Descobertas

- 📈 **Encadeamento** mantém performance estável com alto fator de carga
- 💾 **Endereçamento Aberto** é mais eficiente em memória
- ⚡ **Função Divisão** geralmente mais rápida
- 🎯 **Função Multiplicação** oferece melhor distribuição
- 📏 **Tamanho da tabela** é crucial para performance do encadeamento

## 📄 Licença

Este projeto é distribuído sob a Licença MIT.

---

**Desenvolvido para a disciplina de Pesquisa e Ordenação**  