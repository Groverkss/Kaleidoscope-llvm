#ifndef _LEXER_H_
#define _LEXER_H_

#include <cctype>
#include <iostream>
#include <string>

/**
 * Token Conventions:
 *
 * Non-negative number: token consisting of ASCII character.
 * Negative number represented by enum: other tokens.
 */
enum Token {
    tokEOF = -1,

    tokDef = -2,
    tokExtern = -3,

    tokIdentifier = -4,
    tokNum = -5,
};

extern std::string TokenIdentifier;
extern double TokenNum;

/**
 * Returns the type of next availale token.
 * If token is an identifier, it is stored in TokenIde.
 * If token is a number, it is stored in TokenNum.
 */
int getNextToken();

#endif
