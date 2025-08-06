cmake_minimum_required(VERSION 3.27)
project(projetofinal C)

# Definir flags do compilador C
SET(CMAKE_C_FLAGS "-Wall -Wextra -g -Wvla -Wpedantic -Wdeclaration-after-statement -lm")

# Definir padrão C
set(CMAKE_C_STANDARD 11)

# Adicionar executável
add_executable(projetofinal main.c)