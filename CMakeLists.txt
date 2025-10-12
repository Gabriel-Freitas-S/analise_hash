cmake_minimum_required(VERSION 3.10)

project(analise_hash LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Adiciona o diretório 'include' para que o compilador encontre os cabeçalhos
include_directories(include)

# Encontra todos os arquivos de código-fonte no diretório 'src'
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Adiciona o executável principal
add_executable(analise_hash ${SOURCES})
