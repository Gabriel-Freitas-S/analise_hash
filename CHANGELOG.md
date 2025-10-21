# Changelog

## [1.1.0] - 2024-10-21

### ⚙️ Correções Principais

#### 🔧 Constante de Multiplicação Corrigida
- **PROBLEMA IDENTIFICADO**: O código utilizava a constante da fração áurea (0.6180339887) ao invés da constante especificada no Trabalho 2
- **CORREÇÃO**: Atualizada para `c = 0.63274838` conforme especificação
- **ARQUIVOS AFETADOS**:
  - `include/TabelaEncadeada.hpp` → Constante `CONSTANTE_MULTIPLICACAO`
  - `include/TabelaAberta.hpp` → Constante `CONSTANTE_MULTIPLICACAO`
  - `README.md` → Documentação da fórmula

#### 📏 Documentação Melhorada
- **main.cpp**: Correção de comentários sobre datasets e especificações
- **README.md**: Atualização da constante de multiplicação na documentação
- **Headers**: Melhoria na documentação das funções hash

#### ✨ Código Otimizado
- **CarregadorDados.cpp**: Remoção de métodos desnecessários:
  - `gerarArquivosTrabalho()` - arquivos já existem
  - `gerarRelatorioDatasets()` - funcionalidade redundante
  - `BenchmarkCarregadorDados` - classe não utilizada
- **CarregadorDados.hpp**: Atualização das declarações correspondentes

### ✅ Conformidade com Trabalho 2

#### 📁 Datasets Verificados
- Todos os 6 arquivos de dataset estão presentes e corretos:
  - `numeros_aleatorios_100.txt` (100 elementos)
  - `numeros_aleatorios_500.txt` (500 elementos)
  - `numeros_aleatorios_1000.txt` (1.000 elementos)
  - `numeros_aleatorios_5000.txt` (5.000 elementos)
  - `numeros_aleatorios_10000.txt` (10.000 elementos)
  - `numeros_aleatorios_50000.txt` (50.000 elementos)

#### 🔢 Funções Hash Verificadas
- **Método da Divisão**: `h(k) = k mod p` (✅ implementado corretamente)
- **Método da Multiplicação**: `h(k) = ⌊((k × c) mod 1) × p⌋` com `c = 0.63274838` (✅ corrigido)

#### 📋 Estrutura do Projeto
- Todas as classes requeridas implementadas:
  - `TabelaEncadeada` - Encadeamento para resolução de colisões
  - `TabelaAberta` - Endereçamento aberto com sondagem linear
  - `CarregadorDados` - Gerenciamento de datasets
- Programa principal (`main.cpp`) executando todos os benchmarks especificados

### 🔍 Verificações Realizadas

✔️ **Constantes matemáticas**: Corrigidas conforme especificação  
✔️ **Datasets**: Todos presentes com quantidades corretas  
✔️ **Funções hash**: Implementações corretas  
✔️ **Documentação**: Atualizada e consistente  
✔️ **Código limpo**: Removido código desnecessário  
✔️ **Compilabilidade**: Todos os arquivos compilam sem erros  

---

## [1.0.0] - 2024-10-18

### 🎆 Release Inicial

- Implementação inicial das tabelas hash
- Sistema de benchmarking
- Geração de datasets
- Interface web para visualização dos resultados

---

**Nota**: Todas as correções foram realizadas para garantir total conformidade com os requisitos do **Trabalho 2** da disciplina de Pesquisa e Ordenação.