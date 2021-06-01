#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "LoxLexer.h"
#include "LoxParser.h"
#include "antlr4-runtime.h"
#include "interpreter.h"
#include "resolver.h"
#include "util.h"

using namespace loxgrammar;

void run_file(const std::string &file);
void run_prompt();
void run(antlr4::ANTLRInputStream &input, Interpreter &interpreter);

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
    antlr4::ANTLRFileStream input;
    input.loadFromFile(file);
    try {
        Interpreter interpreter;
        run(input, interpreter);
    } catch (const InterpreterError &e) {
        /*
        if (e.token) {
        // This seems to still crash with the file input stream?
            std::cout << "[error] at " << e.token->getLine() << ":"
                      << e.token->getCharPositionInLine() << ": " << e.message << "\n";
        } else {
        */
        std::cout << "[error] " << e.message << "\n";
        //}
        std::exit(1);
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
        antlr4::ANTLRInputStream input(line);
        try {
            run(input, interpreter);
        } catch (const InterpreterError &e) {
            std::cout << "[error] " << e.message << "\n";
        }
        std::cout << "> ";
    }
}

void run(antlr4::ANTLRInputStream &input, Interpreter &interpreter)
{
    LoxLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    tokens.fill();

    for (const auto &t : tokens.getTokens()) {
        std::cout << t->toString() << "\n";
    }

    // TODO: handle errors in parser

    LoxParser parser(&tokens);
    antlr4::tree::ParseTree *tree = parser.file();

    std::cout << tree->toStringTree(&parser) << "\n";

    Resolver resolver(interpreter);
    resolver.visit(tree);

    interpreter.visit(tree);
}
