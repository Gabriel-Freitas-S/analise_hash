cmake_minimum_required(VERSION 3.15)
project(analise_hash 
    VERSION 1.0.0 
    DESCRIPTION "Análise Comparativa de Soluções Baseadas em Hashing"
    LANGUAGES CXX
)

# Configurações do padrão C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Definir tipo de build padrão
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Opções de configuração
option(ENABLE_WARNINGS "Habilitar warnings adicionais" ON)

# Detecção de compilador e configurações específicas
if(MSVC)
    # Configurações para Visual Studio
    set(CMAKE_CXX_FLAGS "/EHsc /utf-8")
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG /MD")
    
    if(ENABLE_WARNINGS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
else()
    # Configurações para GCC/Clang
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")
    
    if(ENABLE_WARNINGS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    endif()
endif()

# Incluir diretórios de header
include_directories(${PROJECT_SOURCE_DIR}/include)

# Encontrar arquivos fonte
set(SOURCES
    src/main.cpp
    src/TabelaHashEncadeada.cpp
    src/TabelaHashAberta.cpp
    src/GeradorDados.cpp
)

set(HEADERS
    include/TabelaHashEncadeada.hpp
    include/TabelaHashAberta.hpp
    include/GeradorDados.hpp
)

# Criar executável principal
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Definir propriedades do target
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "analise_hash"
)

# Verificar dependências do sistema
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# Verificar se filesystem precisa de link explícito (GCC < 9)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs)
endif()

# Configurações adicionais para diferentes sistemas
if(WIN32)
    # Configurações específicas do Windows
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
        WIN32_LEAN_AND_MEAN
    )
endif()

# Informações de build
message(STATUS "=== Configuração do Build ===")
message(STATUS "Projeto: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Tipo de build: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compilador: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Padrão C++: ${CMAKE_CXX_STANDARD}")
message(STATUS "Sistema: ${CMAKE_SYSTEM_NAME}")
message(STATUS "==============================")

# Verificar se os arquivos de dados existem
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/data")
    message(WARNING "Diretório 'data/' não encontrado. Certifique-se de que os arquivos de dados estejam presentes.")
endif()

# Configurar flags de compilação baseadas no build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_MODE=1)
    message(STATUS "Modo DEBUG ativado")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG=1)
    message(STATUS "Modo RELEASE ativado - máxima performance")
endif()