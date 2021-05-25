#include <iostream>

#include "lexer.h"

void printTokens() {
    while (true) {
        auto tokenType = getNextToken();

        switch (tokenType) {
            case tokEOF:
                std::cout << "EOF\n";
                return;
            case tokDef:
                std::cout << "def\n";
                break;
            case tokExtern:
                std::cout << "extern\n";
                break;
            case tokIdentifier:
                std::cout << TokenIdentifier << "\n";
                break;
            case tokNum:
                std::cout << TokenNum << "\n";
                break;
            default:
                std::cout << (char)tokenType << "\n";
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./kaleidoscope <file>\n";
        return 1;
    }

    std::freopen(argv[1], "r", stdin);

    printTokens();
}
