cmake_minimum_required(VERSION 3.16)
project(Chromafall VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Найди SFML 3
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System Audio)

# Собираем исходники
file(GLOB_RECURSE SOURCES src/*.cpp)

# Исполняемый файл
add_executable(Chromafall ${SOURCES})

# SFML заголовки
target_include_directories(Chromafall PRIVATE include)

target_link_libraries(Chromafall PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Audio
)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})