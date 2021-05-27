#include "lexer.h"

std::string TokenIdentifier;
double TokenNum;

int getNextToken() {
    static int lastChar = ' ';

    while (std::isspace(lastChar)) {
        lastChar = getchar();
    }

    if (std::isalpha(lastChar)) {
        TokenIdentifier = "";
        while (std::isalpha(lastChar)) {
            TokenIdentifier += lastChar;
            lastChar = getchar();
        }

        if (TokenIdentifier == "def") {
            return tokDef;
        }

        if (TokenIdentifier == "extern") {
            return tokExtern;
        }

        return tokIdentifier;
    }

    if (std::isdigit(lastChar)) {
        std::string num;
        while (std::isdigit(lastChar)) {
            num += lastChar;
            lastChar = getchar();
        }

        if (lastChar != '.') {
            TokenNum = std::stod(num);
            return tokNum;
        }

        num += lastChar;
        lastChar = getchar();

        while (std::isdigit(lastChar)) {
            num += lastChar;
            lastChar = getchar();
        }

        TokenNum = std::stod(num);
        return tokNum;
    }

    if (lastChar == '#') {
        while (lastChar != '\n') {
            if (lastChar == EOF) {
                return tokEOF;
            }

            lastChar = getchar();
        }

        lastChar = getchar();
        return getNextToken();
    }

    if (lastChar == EOF) {
        return tokEOF;
    }

    auto thisChar = lastChar;
    lastChar = getchar();
    return thisChar;
}
