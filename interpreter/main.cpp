#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "ast_printer.h"
#include "expr.h"
#include "interpreter.h"
#include "parser.h"
#include "scanner.h"
#include "token.h"
#include "util.h"

void run_file(const std::string &file);
void run_prompt();
void run(const std::string &source);

int main(int argc, char **argv)
{
    if (argc > 2) {
        std::cout << "Usage: interpreter [script]\n";
        return 1;
    }

    if (argc == 2) {
        run_file(argv[1]);
    } else {
        run_prompt();
    }

    return 0;
}

void run_file(const std::string &file)
{
    try {
        run(get_file_content(file));
        if (had_error) {
            std::exit(1);
        }
    } catch (const std::runtime_error &e) {
        std::cout << "interpreter error: " << e.what() << "\n";
        std::exit(1);
    }
}

void run_prompt()
{
    std::cout << "> ";
    std::string line;
    while (std::getline(std::cin, line)) {
        run(line);
        std::cout << "> ";
        had_error = false;
    }
}

void run(const std::string &source)
{
    Scanner scanner(source);
    const auto &tokens = scanner.scan_tokens();

    for (const auto &t : tokens) {
        std::cout << t << "\n";
    }

    Parser parser(tokens);
    const auto expr = parser.parse();

    if (had_error) {
        return;
    }

    ASTPrinter printer;
    std::cout << "AST: " << printer.print(*expr) << "\n";

    Interpreter interpreter;
    try {
        std::any result = interpreter.evaluate(*expr);
        std::cout << "Result: ";
        if (result.has_value()) {
            if (result.type() == typeid(float)) {
                std::cout << std::any_cast<float>(result) << "\n";
            } else if (result.type() == typeid(std::string)) {
                std::cout << std::any_cast<std::string>(result) << "\n";
            } else if (result.type() == typeid(bool)) {
                std::cout << std::any_cast<bool>(result) << "\n";
            } else {
                std::cout << "[error]: Unsupported result type!?\n";
            }
        } else {
            std::cout << "nil";
        }
    } catch (const InterpreterError &e) {
        error(e.token, e.message);
    }
}

