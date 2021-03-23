#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "ast_printer.h"
#include "expr.h"
#include "interpreter.h"
#include "parser.h"
#include "resolver.h"
#include "scanner.h"
#include "token.h"
#include "util.h"

void run_file(const std::string &file);
void run_prompt();
void run(const std::string &source, Interpreter &interpreter);

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
        Interpreter interpreter;
        run(get_file_content(file), interpreter);
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
    Interpreter interpreter;
    while (std::getline(std::cin, line)) {
        run(line, interpreter);
        std::cout << "> ";
        had_error = false;
    }
}

void run(const std::string &source, Interpreter &interpreter)
{
    Scanner scanner(source);
    const auto &tokens = scanner.scan_tokens();

    for (const auto &t : tokens) {
        std::cout << t << "\n";
    }

    Parser parser(tokens);
    const auto statements = parser.parse();

    if (had_error) {
        return;
    }

    Resolver resolver(interpreter);
    resolver.resolve(statements);

    if (had_error) {
        return;
    }

    ProgramPrinter printer;
    std::cout << "Program:\n" << printer.print(statements) << "------\n";

    interpreter.evaluate(statements);
}

