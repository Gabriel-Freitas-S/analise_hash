cmake_minimum_required(VERSION 3.15)
project(analise_hash 
    VERSION 1.0.0 
    DESCRIPTION "Análise Comparativa de Soluções Baseadas em Hashing"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

option(ENABLE_WARNINGS "Habilitar warnings adicionais" ON)

# Flags específicas para tornar o executável mais portátil em Windows
if(WIN32)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # Linkagem estática das libs C++/gcc e pthread (quando disponível)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++ -Wl,-Bstatic -lpthread -Wl,-Bdynamic")
    elseif(MSVC)
        # Runtime estático (/MT) em Release
        string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    endif()
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "/EHsc /utf-8")
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /DNDEBUG")
    if(ENABLE_WARNINGS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
else()
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    if(ENABLE_WARNINGS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    endif()
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES
    src/main.cpp
    src/TabelaEncadeada.cpp
    src/TabelaAberta.cpp
    src/CarregadorDados.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "analise_hash")

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs)
endif()

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

# Copiar pasta data automaticamente para o diretório de execução
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/data"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_SOURCE_DIR}/data"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/data"
    COMMENT "Copiando pasta data para o diretório de execução"
)

# Target para empacotar release portátil
add_custom_target(portable_release ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/portable_release"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${PROJECT_NAME}>" "${CMAKE_BINARY_DIR}/portable_release/analise_hash.exe"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/data" "${CMAKE_BINARY_DIR}/portable_release/data"
    COMMAND ${CMAKE_COMMAND} -E echo "ANALISE COMPARATIVA DE TABELAS HASH - v${PROJECT_VERSION}" > "${CMAKE_BINARY_DIR}/portable_release/README_EXECUTAVEL.txt"
    COMMAND ${CMAKE_COMMAND} -E echo "Como executar: abra o Prompt nesta pasta e execute 'analise_hash.exe'" >> "${CMAKE_BINARY_DIR}/portable_release/README_EXECUTAVEL.txt"
    DEPENDS ${PROJECT_NAME}
    COMMENT "Gerando pasta 'portable_release' com executável e dados"
)

message(STATUS "=== Configuração do Build ===")
message(STATUS "Projeto: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Tipo de build: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compilador: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Sistema: ${CMAKE_SYSTEM_NAME}")
message(STATUS "==============================")