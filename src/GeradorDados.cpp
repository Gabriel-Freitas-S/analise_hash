#include "GeradorDados.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <set>
#include <filesystem>

// Construtor
GeradorDados::GeradorDados(unsigned int seed, int minimo, int maximo)
    : gerador(seed), distribuicao(minimo, maximo) {
    if (minimo >= maximo) {
        throw std::invalid_argument("Valor mínimo deve ser menor que o máximo");
    }
}

// Verifica se arquivo existe
bool GeradorDados::arquivoExiste(const std::string& nomeArquivo) const {
    std::ifstream arquivo(nomeArquivo);
    return arquivo.good();
}

// Remove espaços em branco
std::string GeradorDados::trim(const std::string& str) const {
    size_t primeiro = str.find_first_not_of(" \t
");
    if (primeiro == std::string::npos) {
        return "";
    }
    size_t ultimo = str.find_last_not_of(" \t
");
    return str.substr(primeiro, (ultimo - primeiro + 1));
}

// Gera números aleatórios únicos
std::vector<int> GeradorDados::gerarNumerosAleatorios(size_t quantidade) {
    if (quantidade == 0) {
        throw std::invalid_argument("Quantidade deve ser maior que zero");
    }
    
    // Para grandes quantidades, usar set para evitar duplicatas
    if (quantidade > 10000) {
        return gerarNumerosAleatoriosComRepeticao(quantidade);
    }
    
    std::set<int> numerosUnicos;
    std::vector<int> resultado;
    resultado.reserve(quantidade);
    
    // Gera números únicos
    while (numerosUnicos.size() < quantidade) {
        int numero = distribuicao(gerador);
        if (numerosUnicos.insert(numero).second) {
            resultado.push_back(numero);
        }
    }
    
    return resultado;
}

// Gera números aleatórios com possível repetição
std::vector<int> GeradorDados::gerarNumerosAleatoriosComRepeticao(size_t quantidade) {
    if (quantidade == 0) {
        throw std::invalid_argument("Quantidade deve ser maior que zero");
    }
    
    std::vector<int> numeros;
    numeros.reserve(quantidade);
    
    for (size_t i = 0; i < quantidade; ++i) {
        numeros.push_back(distribuicao(gerador));
    }
    
    return numeros;
}

// Salva números em arquivo
bool GeradorDados::salvarEmArquivo(const std::vector<int>& numeros, 
                                   const std::string& nomeArquivo) {
    if (numeros.empty()) {
        std::cerr << "Erro: Vetor vazio, não há dados para salvar.
";
        return false;
    }
    
    // Cria diretório se não existir
    std::filesystem::path caminhoArquivo(nomeArquivo);
    if (caminhoArquivo.has_parent_path()) {
        std::filesystem::create_directories(caminhoArquivo.parent_path());
    }
    
    std::ofstream arquivo(nomeArquivo);
    
    if (!arquivo.is_open()) {
        throw std::runtime_error("Erro ao criar arquivo: " + nomeArquivo);
    }
    
    // Formato: primeira linha contém a quantidade
    arquivo << numeros.size() << "
";
    
    // Cada número em uma linha
    for (const int& numero : numeros) {
        arquivo << numero << "
";
    }
    
    arquivo.close();
    
    std::cout << "✓ Arquivo salvo: " << nomeArquivo 
              << " (" << numeros.size() << " números)
";
    
    return true;
}

// Carrega números de arquivo
std::vector<int> GeradorDados::carregarDeArquivo(const std::string& nomeArquivo) {
    if (!arquivoExiste(nomeArquivo)) {
        throw std::runtime_error("Arquivo não encontrado: " + nomeArquivo);
    }
    
    std::ifstream arquivo(nomeArquivo);
    
    if (!arquivo.is_open()) {
        throw std::runtime_error("Erro ao abrir arquivo: " + nomeArquivo);
    }
    
    std::vector<int> numeros;
    std::string linha;
    
    // Primeira linha: quantidade de números
    if (!std::getline(arquivo, linha)) {
        throw std::runtime_error("Arquivo vazio ou formato inválido: " + nomeArquivo);
    }
    
    linha = trim(linha);
    size_t quantidade = 0;
    
    try {
        quantidade = std::stoull(linha);
    } catch (const std::exception& e) {
        throw std::runtime_error("Formato inválido na primeira linha: " + nomeArquivo);
    }
    
    if (quantidade == 0) {
        throw std::runtime_error("Quantidade de números não pode ser zero");
    }
    
    numeros.reserve(quantidade);
    size_t linhaAtual = 1;
    
    // Lê os números
    while (std::getline(arquivo, linha) && numeros.size() < quantidade) {
        linhaAtual++;
        linha = trim(linha);
        
        if (linha.empty()) {
            continue; // Ignora linhas vazias
        }
        
        try {
            int numero = std::stoi(linha);
            numeros.push_back(numero);
        } catch (const std::exception& e) {
            std::cerr << "Aviso: Linha " << linhaAtual 
                     << " inválida, pulando...
";
        }
    }
    
    arquivo.close();
    
    // Valida se a quantidade está correta
    if (numeros.size() != quantidade) {
        std::cerr << "Aviso: Esperado " << quantidade 
                 << " números, mas leu " << numeros.size() << "
";
    }
    
    std::cout << "✓ Arquivo carregado: " << nomeArquivo 
              << " (" << numeros.size() << " números)
";
    
    return numeros;
}

// Gera todos os arquivos necessários para o trabalho
void GeradorDados::gerarArquivosTrabalho(const std::string& diretorio) {
    std::cout << "
=== Gerando Arquivos de Dados ===

";
    
    // Cria diretório se não existir
    std::filesystem::create_directories(diretorio);
    
    // Quantidades conforme especificação do trabalho
    std::vector<size_t> quantidades = {100, 500, 1000, 5000, 10000, 50000};
    
    try {
        // Gera arquivos de dados para inserção
        for (size_t quantidade : quantidades) {
            std::string nomeArquivo = diretorio + "/numeros_" + 
                                     std::to_string(quantidade) + ".txt";
            
            std::cout << "Gerando " << quantidade << " números...
";
            auto numeros = gerarNumerosAleatoriosComRepeticao(quantidade);
            salvarEmArquivo(numeros, nomeArquivo);
        }
        
        // Gera arquivo de busca com 1000 números
        std::cout << "
Gerando 1000 números para busca...
";
        auto numerosBusca = gerarNumerosAleatoriosComRepeticao(1000);
        salvarEmArquivo(numerosBusca, diretorio + "/busca_1000.txt");
        
        std::cout << "
✓ Todos os arquivos foram gerados com sucesso!
";
        std::cout << "Diretório: " << diretorio << "

";
        
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Erro ao gerar arquivos: ") + e.what());
    }
}

// Redefine a seed
void GeradorDados::redefinirSeed(unsigned int novaSeed) {
    gerador.seed(novaSeed);
}

// Redefine o intervalo
void GeradorDados::redefinirIntervalo(int minimo, int maximo) {
    if (minimo >= maximo) {
        throw std::invalid_argument("Valor mínimo deve ser menor que o máximo");
    }
    distribuicao = std::uniform_int_distribution<int>(minimo, maximo);
}

// Valida estrutura do arquivo
bool GeradorDados::validarArquivo(const std::string& nomeArquivo) const {
    try {
        if (!arquivoExiste(nomeArquivo)) {
            std::cerr << "Arquivo não existe: " << nomeArquivo << "
";
            return false;
        }
        
        std::ifstream arquivo(nomeArquivo);
        std::string linha;
        
        // Valida primeira linha (quantidade)
        if (!std::getline(arquivo, linha)) {
            std::cerr << "Arquivo vazio
";
            return false;
        }
        
        size_t quantidade = std::stoull(trim(linha));
        size_t contagem = 0;
        
        // Conta números válidos
        while (std::getline(arquivo, linha)) {
            linha = trim(linha);
            if (!linha.empty()) {
                try {
                    std::stoi(linha); // Tenta converter
                    contagem++;
                } catch (...) {
                    std::cerr << "Número inválido na linha " << (contagem + 2) << "
";
                    return false;
                }
            }
        }
        
        if (contagem != quantidade) {
            std::cerr << "Quantidade esperada: " << quantidade 
                     << ", encontrada: " << contagem << "
";
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Erro ao validar arquivo: " << e.what() << "
";
        return false;
    }
}