# Changelog

## [1.1.0] - 2024-10-18

### ✨ Melhorias

- **Renomeação de arquivos para maior clareza:**
  - `GeradorDados` → `CarregadorDados` (nome mais apropriado para sua função principal)
  - `TabelaHashEncadeada` → `TabelaEncadeada` (nome mais limpo e direto)
  - `TabelaHashAberta` → `TabelaAberta` (nome mais conciso)

### 🧹 Limpeza

- **Removidos arquivos desncessários:**
  - `CMakeLists.msvc.txt` (funcionalidade já integrada no CMakeLists.txt principal)
  - `CMakeLists_simple.txt` (redundância desncessária)
  - Arquivos antigos com nomes menos claros

### 🔄 Atualizações

- **CMakeLists.txt:** Atualizado para usar novos nomes de arquivos
- **main.cpp:** Includes atualizados para os novos headers
- **README.md:** Documentação completamente reformulada com:
  - Estrutura mais clara e organizada
  - Novos nomes de arquivos refletidos
  - Melhor formatação com emojis e tabelas
  - Informações mais detalhadas sobre implementação

### 📝 Detalhes Técnicos

- Todos os includes e referências foram atualizados
- Compatibilidade mantida com todas as funcionalidades existentes
- Nenhuma alteração na lógica de implementação
- Apenas melhoria na organização e clareza do código

### 📦 Estrutura Final

```
analise_hash/
├── include/
│   ├── CarregadorDados.hpp      # Carregamento de datasets
│   ├── TabelaEncadeada.hpp      # Tabela hash com encadeamento
│   └── TabelaAberta.hpp         # Tabela hash com endereçamento aberto
├── src/
│   ├── main.cpp                # Programa principal
│   ├── CarregadorDados.cpp      # Implementação do carregador
│   ├── TabelaEncadeada.cpp      # Implementação encadeamento
│   └── TabelaAberta.cpp         # Implementação endereçamento aberto
├── data/                        # Datasets de teste
├── CMakeLists.txt               # Configuração de build
├── README.md                    # Documentação
└── CHANGELOG.md                 # Este arquivo
```