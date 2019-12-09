#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "scanner.h"
#include "util.h"

void run_file(const std::string &file);
void run_prompt();
void run(const std::string &text);

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
    }
}

void run(const std::string &text)
{
    std::cout << "Running: " << text << "\n";
}

