cmake_minimum_required(VERSION 3.5)
project(interpreter)

if (NOT WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

add_executable(interpreter main.cpp util.cpp scanner.cpp
    token.cpp expr.cpp ast_printer.cpp)

set_target_properties(interpreter PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON)

