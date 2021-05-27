#include <iostream>

#include "lexer.h"
#include "parser.h"

void printTokens() {
    while (true) {
        auto tokenType = getNextToken();

        switch (tokenType) {
            case tokEOF:
                std::cout << "EOF";
                return;
            case tokDef:
                std::cout << "def";
                break;
            case tokExtern:
                std::cout << "extern";
                break;
            case tokIdentifier:
                std::cout << TokenIdentifier;
                break;
            case tokNum:
                std::cout << TokenNum;
                break;
            default:
                std::cout << (char)tokenType;
        }

        std::cout << " --> " << tokenType << "\n";
    }
}

void interpreter(bool interpret) { parse(interpret); }

int main(int argc, char *argv[]) {
    if (argc == 1) {
        interpreter(true);
        return 0;
    }

    if (argc != 2) {
        std::cerr << "Usage: ./kaleidoscope <file>\n";
        return 1;
    }

    std::freopen(argv[1], "r", stdin);

    interpreter(false);
}
