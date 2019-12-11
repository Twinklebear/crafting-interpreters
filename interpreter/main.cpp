#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "ast_printer.h"
#include "expr.h"
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

    auto expr = std::make_shared<Binary>(
        std::make_shared<Unary>(Token(TokenType::MINUS, "-", 1),
                                std::make_shared<Literal>(std::any(123.f))),
        Token(TokenType::STAR, "*", 1),
        std::make_shared<Grouping>(std::make_shared<Literal>(std::any(45.67f))));
    ASTPrinter printer;
    std::cout << printer.print(*expr) << "\n";
    return 0;

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
}

