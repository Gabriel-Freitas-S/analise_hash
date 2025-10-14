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
option(ENABLE_TESTING "Habilitar testes unitários" OFF)
option(ENABLE_BENCHMARKS "Habilitar benchmarks detalhados" ON)
option(ENABLE_WARNINGS "Habilitar warnings adicionais" ON)
option(ENABLE_SANITIZERS "Habilitar sanitizers em Debug" ON)

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
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")
    
    if(ENABLE_WARNINGS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion")
    endif()
    
    # Sanitizers para Debug (apenas em sistemas Unix)
    if(ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -fno-omit-frame-pointer")
        set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address,undefined")
    endif()
endif()

# Incluir diretórios de header
include_directories(${PROJECT_SOURCE_DIR}/include)

# Encontrar todos os arquivos fonte
file(GLOB_RECURSE SOURCES 
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.cxx"
)

file(GLOB_RECURSE HEADERS 
    "${PROJECT_SOURCE_DIR}/include/*.hpp"
    "${PROJECT_SOURCE_DIR}/include/*.h"
)

# Criar executável principal
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Definir propriedades do target
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "analise_hash"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
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
elseif(UNIX AND NOT APPLE)
    # Configurações para Linux
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
endif()

# Instalação
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    COMPONENT Runtime
)

# Instalar arquivos de dados se existirem
if(EXISTS "${PROJECT_SOURCE_DIR}/data")
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/data/"
        DESTINATION share/${PROJECT_NAME}/data
        FILES_MATCHING PATTERN "*.txt"
        COMPONENT Data
    )
endif()

# Instalar documentação
if(EXISTS "${PROJECT_SOURCE_DIR}/README.md")
    install(FILES "${PROJECT_SOURCE_DIR}/README.md"
        DESTINATION share/${PROJECT_NAME}/doc
        COMPONENT Documentation
    )
endif()

# Target para gerar arquivos de dados
add_custom_target(generate_data
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME} --generate-data
    DEPENDS ${PROJECT_NAME}
    COMMENT "Gerando arquivos de dados para testes"
    VERBATIM
)

# Target para limpar arquivos gerados
add_custom_target(clean_results
    COMMAND ${CMAKE_COMMAND} -E remove -f 
        "${CMAKE_CURRENT_BINARY_DIR}/*.csv"
        "${CMAKE_CURRENT_BINARY_DIR}/*.txt"
    COMMENT "Limpando arquivos de resultado"
    VERBATIM
)

# Target para executar benchmarks completos
add_custom_target(benchmark
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    COMMENT "Executando benchmarks completos"
    VERBATIM
)

# Informações de build
message(STATUS "=== Configuração do Build ===")
message(STATUS "Projeto: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Tipo de build: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compilador: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Padrão C++: ${CMAKE_CXX_STANDARD}")
message(STATUS "Sistema: ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}")
message(STATUS "Warnings: ${ENABLE_WARNINGS}")
message(STATUS "Sanitizers: ${ENABLE_SANITIZERS}")
message(STATUS "Benchmarks: ${ENABLE_BENCHMARKS}")
message(STATUS "Diretório de saída: ${CMAKE_BINARY_DIR}/bin")
message(STATUS "==============================")

# Verificar se os arquivos de dados existem
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/data")
    message(WARNING "Diretório 'data/' não encontrado. Use 'make generate_data' após a compilação.")
endif()

# Configurar flags de compilação baseadas no build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_MODE=1)
    message(STATUS "Modo DEBUG ativado - desempenho pode ser reduzido")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG=1)
    message(STATUS "Modo RELEASE ativado - máxima performance")
endif()