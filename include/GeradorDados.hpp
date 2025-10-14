#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

/**
 * @brief Classe responsável por carregar dados de arquivos de texto
 * 
 * Esta classe fornece funcionalidades para:
 * - Carregar dados de arquivos existentes na pasta data/
 * - Validar a integridade dos arquivos
 * - Listar arquivos disponíveis
 */
class GeradorDados {
private:
    /**
     * @brief Valida se um arquivo existe e pode ser lido
     * @param nomeArquivo Caminho do arquivo a ser validado
     * @return true se o arquivo existe e é legível, false caso contrário
     */
    bool arquivoExiste(const std::string& nomeArquivo) const;
    
    /**
     * @brief Remove espaços em branco de uma string
     * @param str String a ser processada
     * @return String sem espaços nas extremidades
     */
    std::string trim(const std::string& str) const;

public:
    /**
     * @brief Construtor padrão
     */
    GeradorDados() = default;
    
    /**
     * @brief Carrega números de um arquivo de texto
     * 
     * Formato esperado:
     * - Primeira linha: quantidade de números (opcional, usado para validação)
     * - Linhas seguintes: um número por linha
     * 
     * @param nomeArquivo Caminho do arquivo de entrada
     * @return Vetor com os números carregados
     * @throws std::runtime_error se o arquivo não existir ou formato inválido
     */
    std::vector<int> carregarDeArquivo(const std::string& nomeArquivo);
    
    /**
     * @brief Valida a estrutura de um arquivo de dados
     * @param nomeArquivo Arquivo a ser validado
     * @return true se o arquivo está no formato correto
     */
    bool validarArquivo(const std::string& nomeArquivo) const;
    
    /**
     * @brief Lista todos os arquivos disponíveis na pasta data/
     * @return Vetor com os caminhos dos arquivos encontrados
     */
    std::vector<std::string> listarArquivosDisponiveis() const;
    
    /**
     * @brief Obtém estatísticas básicas de um arquivo
     * @param nomeArquivo Caminho do arquivo
     */
    void exibirEstatisticas(const std::string& nomeArquivo) const;
};