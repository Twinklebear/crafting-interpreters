cmake_minimum_required(VERSION 3.5)
project(interpreter)

if (NOT WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

find_package(Python COMPONENTS Interpreter)

add_custom_command(OUTPUT expr.cpp
    COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/gen_expr.py
        ${CMAKE_CURRENT_BINARY_DIR}/expr
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/gen_expr.py)

add_executable(interpreter
    main.cpp
    util.cpp
    scanner.cpp
    token.cpp
    ast_printer.cpp
    parser.cpp
    interpreter.cpp
   ${CMAKE_CURRENT_BINARY_DIR}/expr.cpp)

target_include_directories(interpreter PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_LIST_DIR})

set_target_properties(interpreter PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON)

