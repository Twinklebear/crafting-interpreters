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

# Add the Lox Grammar
antlr_target(LoxGrammar Lox.g4
    PACKAGE loxgrammar
    LEXER
    PARSER
    VISITOR)


add_executable(interpreter
    main.cpp
    util.cpp
    interpreter.cpp
    environment.cpp
    lox_callable.cpp
    resolver.cpp
    lox_class.cpp
    ${ANTLR_LoxGrammar_CXX_OUTPUTS})

target_include_directories(interpreter PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_LIST_DIR}
    ${ANTLR_LoxGrammar_OUTPUT_DIR})

target_link_libraries(interpreter antlr4_static)

set_target_properties(interpreter PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON)

