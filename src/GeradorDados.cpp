#include "GeradorDados.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <set>
#include <unordered_set>
#include <numeric>
#include <chrono>
#include <iomanip>

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
    size_t quantidadeEsperada = 0;
    
    try {
        quantidadeEsperada = std::stoull(linha);
    } catch (const std::exception& e) {
        throw std::runtime_error("Formato inválido na primeira linha: " + nomeArquivo);
    }
    
    if (quantidadeEsperada == 0) {
        throw std::runtime_error("Quantidade de números não pode ser zero");
    }
    
    numeros.reserve(quantidadeEsperada);
    size_t linhaAtual = 1;
    size_t numerosLidos = 0;
    
    // Ler os números
    while (std::getline(arquivo, linha) && numerosLidos < quantidadeEsperada) {
        linhaAtual++;
        linha = trim(linha);
        
        if (linha.empty()) {
            continue; // Ignorar linhas vazias
        }
        
        try {
            int numero = std::stoi(linha);
            numeros.push_back(numero);
            numerosLidos++;
        } catch (const std::exception& e) {
            // Log do erro mas continue processando
            std::cerr << "Aviso: Linha " << linhaAtual 
                     << " inválida (\"" << linha << "\"), ignorando...\n";
        }
    }
    
    arquivo.close();
    
    // Validar se a quantidade está correta
    if (numeros.size() != quantidadeEsperada) {
        std::cerr << "Aviso: Esperado " << quantidadeEsperada 
                 << " números, mas leu " << numeros.size() << "\n";
    }
    
    if (numeros.empty()) {
        throw std::runtime_error("Nenhum número válido foi encontrado no arquivo");
    }
    
    std::cout << "✓ Arquivo carregado: " << nomeArquivo 
              << " (" << numeros.size() << " números)\n";
    
    return numeros;
}

std::vector<int> GeradorDados::gerarNumerosAleatorios(size_t quantidade) {
    if (quantidade == 0) {
        throw std::invalid_argument("Quantidade deve ser maior que zero");
    }
    
    // Para grandes quantidades, usar set para evitar duplicatas
    if (quantidade > 10000) {
        return gerarNumerosAleatoriosComRepeticao(quantidade);
    }
    
    std::unordered_set<int> numerosUnicos;
    std::vector<int> resultado;
    resultado.reserve(quantidade);
    
    // Gerar números únicos
    while (numerosUnicos.size() < quantidade) {
        int numero = distribuicao(gerador);
        if (numerosUnicos.insert(numero).second) {
            resultado.push_back(numero);
        }
    }
    
    return resultado;
}

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

bool GeradorDados::salvarEmArquivo(const std::vector<int>& numeros, 
                                   const std::string& nomeArquivo) {
    if (numeros.empty()) {
        std::cerr << "Erro: Vetor vazio, não há dados para salvar.\n";
        return false;
    }
    
    // Criar diretório se não existir
    std::filesystem::path caminhoArquivo(nomeArquivo);
    if (caminhoArquivo.has_parent_path()) {
        std::filesystem::create_directories(caminhoArquivo.parent_path());
    }
    
    std::ofstream arquivo(nomeArquivo);
    
    if (!arquivo.is_open()) {
        throw std::runtime_error("Erro ao criar arquivo: " + nomeArquivo);
    }
    
    // Formato: primeira linha contém a quantidade
    arquivo << numeros.size() << "\n";
    
    // Cada número em uma linha
    for (const int& numero : numeros) {
        arquivo << numero << "\n";
    }
    
    arquivo.close();
    
    std::cout << "✓ Arquivo salvo: " << nomeArquivo 
              << " (" << numeros.size() << " números)\n";
    
    return true;
}

bool GeradorDados::validarArquivo(const std::string& nomeArquivo) const {
    try {
        if (!arquivoExiste(nomeArquivo)) {
            std::cerr << "Arquivo não existe: " << nomeArquivo << "\n";
            return false;
        }
        
        std::ifstream arquivo(nomeArquivo);
        std::string linha;
        
        // Validar primeira linha (quantidade)
        if (!std::getline(arquivo, linha)) {
            std::cerr << "Arquivo vazio\n";
            return false;
        }
        
        size_t quantidade = std::stoull(trim(linha));
        size_t contagem = 0;
        
        // Contar números válidos
        while (std::getline(arquivo, linha)) {
            linha = trim(linha);
            if (!linha.empty()) {
                try {
                    std::stoi(linha); // Tentar converter
                    contagem++;
                } catch (...) {
                    std::cerr << "Número inválido na linha " << (contagem + 2) << "\n";
                    return false;
                }
            }
        }
        
        if (contagem != quantidade) {
            std::cerr << "Quantidade esperada: " << quantidade 
                     << ", encontrada: " << contagem << "\n";
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Erro ao validar arquivo: " << e.what() << "\n";
        return false;
    }
}

void GeradorDados::exibirEstatisticas(const std::string& nomeArquivo) const {
    try {
        auto info = const_cast<GeradorDados*>(this)->analisarDataset(nomeArquivo);
        
        std::cout << "\n=== Estatísticas do Dataset ===\n";
        std::cout << "Arquivo: " << info.nomeArquivo << "\n";
        std::cout << "Quantidade: " << info.quantidade << " números\n";
        std::cout << "Intervalo: [" << info.minimo << ", " << info.maximo << "]\n";
        std::cout << "Média: " << std::fixed << std::setprecision(2) << info.media << "\n";
        std::cout << "Duplicatas: " << (info.temDuplicatas ? "Sim" : "Não");
        if (info.temDuplicatas) {
            std::cout << " (" << info.numDuplicatas << " duplicatas)";
        }
        std::cout << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Erro ao analisar estatísticas: " << e.what() << "\n";
    }
}

GeradorDados::InfoDataset GeradorDados::analisarDataset(const std::string& nomeArquivo) {
    InfoDataset info;
    info.nomeArquivo = nomeArquivo;
    
    auto numeros = carregarDeArquivo(nomeArquivo);
    info.quantidade = numeros.size();
    
    if (numeros.empty()) {
        throw std::runtime_error("Dataset vazio");
    }
    
    // Calcular mínimo, máximo e média
    info.minimo = *std::min_element(numeros.begin(), numeros.end());
    info.maximo = *std::max_element(numeros.begin(), numeros.end());
    
    long long soma = std::accumulate(numeros.begin(), numeros.end(), 0LL);
    info.media = static_cast<double>(soma) / numeros.size();
    
    // Verificar duplicatas
    std::unordered_set<int> unicos;
    info.numDuplicatas = 0;
    
    for (int numero : numeros) {
        if (!unicos.insert(numero).second) {
            info.numDuplicatas++;
        }
    }
    
    info.temDuplicatas = (info.numDuplicatas > 0);
    
    return info;
}

void GeradorDados::gerarRelatorioDatasets() const {
    auto arquivos = listarArquivosDisponiveis();
    
    if (arquivos.empty()) {
        std::cout << "Nenhum arquivo encontrado no diretório data/\n";
        return;
    }
    
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "                       RELATÓRIO DOS DATASETS\n";
    std::cout << std::string(80, '=') << "\n";
    
    for (const std::string& arquivo : arquivos) {
        try {
            auto info = const_cast<GeradorDados*>(this)->analisarDataset(arquivo);
            
            std::cout << "\nArquivo: " << std::filesystem::path(arquivo).filename() << "\n";
            std::cout << "  Elementos: " << info.quantidade << "\n";
            std::cout << "  Intervalo: [" << info.minimo << ", " << info.maximo << "]\n";
            std::cout << "  Média: " << std::fixed << std::setprecision(2) << info.media << "\n";
            std::cout << "  Duplicatas: " << (info.temDuplicatas ? "Sim" : "Não");
            if (info.temDuplicatas) {
                std::cout << " (" << info.numDuplicatas << ")"; 
            }
            std::cout << "\n";
            
        } catch (const std::exception& e) {
            std::cout << "\nErro ao analisar " 
                     << std::filesystem::path(arquivo).filename() 
                     << ": " << e.what() << "\n";
        }
    }
    
    std::cout << std::string(80, '=') << "\n";
}

void GeradorDados::gerarArquivosTrabalho(const std::string& diretorio) {
    std::cout << "\n=== Gerando Arquivos de Dados ===\n";
    
    // Criar diretório se não existir
    std::filesystem::create_directories(diretorio);
    
    // Quantidades conforme especificação do trabalho
    std::vector<size_t> quantidades = {100, 500, 1000, 5000, 10000, 50000};
    
    try {
        // Gerar arquivos de dados para inserção
        for (size_t quantidade : quantidades) {
            std::string nomeArquivo = diretorio + "/numeros_aleatorios_" + 
                                     std::to_string(quantidade) + ".txt";
            
            std::cout << "Gerando " << quantidade << " números...\n";
            auto numeros = gerarNumerosAleatoriosComRepeticao(quantidade);
            salvarEmArquivo(numeros, nomeArquivo);
        }
        
        // Gerar arquivo de busca com 1000 números
        std::cout << "\nGerando 1000 números para busca...\n";
        auto numerosBusca = gerarNumerosAleatoriosComRepeticao(1000);
        salvarEmArquivo(numerosBusca, diretorio + "/busca_1000.txt");
        
        std::cout << "\n✓ Todos os arquivos foram gerados com sucesso!\n";
        std::cout << "Diretório: " << diretorio << "\n\n";
        
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Erro ao gerar arquivos: ") + e.what());
    }
}

// Implementação da classe de benchmark
void BenchmarkGeradorDados::testarDesempenho(const std::vector<size_t>& quantidades) {
    std::cout << "\n=== Benchmark de Geração de Dados ===\n";
    
    GeradorDados gerador;
    
    for (size_t quantidade : quantidades) {
        std::cout << "\nTestando com " << quantidade << " elementos:\n";
        
        // Testar geração com repetição
        auto inicio = std::chrono::high_resolution_clock::now();
        auto dados = gerador.gerarNumerosAleatoriosComRepeticao(quantidade);
        auto fim = std::chrono::high_resolution_clock::now();
        
        auto duracao = std::chrono::duration_cast<std::chrono::milliseconds>(fim - inicio);
        
        std::cout << "  Geração com repetição: " << duracao.count() << "ms\n";
        
        // Testar geração única (apenas para quantidades menores)
        if (quantidade <= 10000) {
            inicio = std::chrono::high_resolution_clock::now();
            auto dadosUnicos = gerador.gerarNumerosAleatorios(quantidade);
            fim = std::chrono::high_resolution_clock::now();
            
            duracao = std::chrono::duration_cast<std::chrono::milliseconds>(fim - inicio);
            std::cout << "  Geração única: " << duracao.count() << "ms\n";
        }
    }
}