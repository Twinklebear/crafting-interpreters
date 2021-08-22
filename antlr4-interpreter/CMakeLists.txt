cmake_minimum_required(VERSION 3.5)
project(interpreter)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if (NOT WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

# Setup ANTLR
add_definitions(-DANTLR4CPP_STATIC)
set(ANTLR4_WITH_STATIC_CRT OFF)
include(ExternalAntlr4Cpp)
include_directories(${ANTLR4_INCLUDE_DIRS})
find_package(ANTLR REQUIRED)

include(ANTLRGrammarUtil)

# Add the Lox Grammar
antlr_target(lox_grammar
    PACKAGE loxgrammar
    LEXER LoxLexer.g4
    PARSER LoxParser.g4
    VISITOR)

antlr_grammar_util(LoxGrammarUtil
    LEXER LoxLexer.g4
    PARSER LoxParser.g4)

# Generate my AST nodes
find_package(Python COMPONENTS Interpreter)
add_custom_command(OUTPUT expr.cpp
    COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/gen_expr.py
        ${CMAKE_CURRENT_BINARY_DIR}/expr
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/gen_expr.py)

add_executable(interpreter
    main.cpp
    util.cpp
    interpreter.cpp
    resolver.cpp
    environment.cpp
    lox_callable.cpp
    lox_class.cpp
    ast_builder.cpp
    ast_printer.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/expr.cpp)

target_include_directories(interpreter PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(interpreter lox_grammar)

set_target_properties(interpreter PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON)

