#include <cctype>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "lexer.h"

/**
 * Scafolding
 **/

/* Expressions */
class ExprAST {
   public:
    virtual ~ExprAST() {}
};

class NumberExprAST : public ExprAST {
    double Val;

   public:
    NumberExprAST(double Val) : Val(Val) {}
};

class VariableExprAST : public ExprAST {
    std::string Name;

   public:
    VariableExprAST(std::string &Name) : Name(Name) {}
};

class BinaryExprAST : public ExprAST {
    char op;
    std::unique_ptr<ExprAST> LHS, RHS;

   public:
    BinaryExprAST(char op, std::unique_ptr<ExprAST> LHS,
                  std::unique_ptr<ExprAST> RHS)
        : op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;

   public:
    CallExprAST(const std::string &Callee,
                std::vector<std::unique_ptr<ExprAST>> Args)
        : Callee(Callee), Args(std::move(Args)) {}
};

/* Prototypes */
class PrototypeAST {
    std::string Name;
    std::vector<std::string> Args;

   public:
    PrototypeAST(const std::string &name, std::vector<std::string> Args)
        : Name(name), Args(std::move(Args)) {}
};

class FunctionAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<ExprAST> Body;

   public:
    FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                std::unique_ptr<ExprAST> Body)
        : Proto(std::move(Proto)), Body(std::move(Body)) {}
};

/**
 * Parsing
 **/

static int CurTok;
static int getToken() { return CurTok = getNextToken(); }

std::unique_ptr<ExprAST> LogError(const char *Str) {
    fprintf(stderr, "LogError: %s\n", Str);
    return nullptr;
}

std::unique_ptr<PrototypeAST> LogErrorP(const char *Str) {
    LogError(Str);
    return nullptr;
}

static std::unique_ptr<ExprAST> parseNumberExpr() {
    auto result = std::make_unique<NumberExprAST>(TokenNum);
    getToken();
    return std::move(result);
}

/* Function declarations, declared later */
static std::unique_ptr<ExprAST> parseExpr();
static std::unique_ptr<ExprAST> parsePrimary();

static std::unique_ptr<ExprAST> parseParenExpr() {
    getToken();
    auto expr = parseExpr();
    if (!expr) {
        return nullptr;
    }

    if (CurTok != ')') {
        return LogError("Expected ')'");
    }
    getToken();
    return expr;
}

static std::unique_ptr<ExprAST> parseIdentifierExpr() {
    std::string name = TokenIdentifier;

    getToken();

    if (CurTok != '(') {
        /* Variable Ref */
        return std::make_unique<VariableExprAST>(name);
    }

    /* Function call */
    getToken();
    std::vector<std::unique_ptr<ExprAST>> args;
    if (CurTok != ')') {
        while (1) {
            if (auto arg = parseExpr()) {
                args.push_back(std::move(arg));
            } else {
                return nullptr;
            }

            if (CurTok == ')') {
                break;
            }

            if (CurTok != ',') {
                return LogError("Expected ')' or ',' in arguement list");
            }
            getToken();
        }
    }

    getToken();
    return std::make_unique<CallExprAST>(name, std::move(args));
}

std::unique_ptr<ExprAST> parsePrimary() {
    switch (CurTok) {
        case tokIdentifier:
            return parseIdentifierExpr();
        case tokNum:
            return parseNumberExpr();
        case '(':
            parseParenExpr();
        default:
            return LogError("Unkown token when expecting an expression");
    }
}

static std::map<char, int> BinopPrecedence{
    {'<', 10},
    {'+', 20},
    {'-', 20},
    {'*', 40},
};

static int getTokPrecendence() {
    if (!isascii(CurTok)) {
        return -1;
    }

    int tokPrec = BinopPrecedence[CurTok];
    if (tokPrec <= 0) {
        return -1;
    }
    return tokPrec;
}

static std::unique_ptr<ExprAST> parseBinOpRHS(int exprPrec,
                                              std::unique_ptr<ExprAST> LHS);

static std::unique_ptr<ExprAST> parseExpr() {
    auto LHS = parsePrimary();
    if (!LHS) {
        return nullptr;
    }

    return parseBinOpRHS(0, std::move(LHS));
}

static std::unique_ptr<ExprAST> parseBinOpRHS(int exprPrec,
                                              std::unique_ptr<ExprAST> LHS) {
    while (true) {
        int tokPrec = getTokPrecendence();

        if (tokPrec < exprPrec) {
            return LHS;
        }

        int binOp = CurTok;
        getToken();

        auto RHS = parsePrimary();
        if (!RHS) {
            return nullptr;
        }

        int nextPrec = getTokPrecendence();
        if (tokPrec < nextPrec) {
            RHS = parseBinOpRHS(tokPrec + 1, std::move(RHS));
            if (!RHS) {
                return nullptr;
            }
        }

        LHS = std::make_unique<BinaryExprAST>(binOp, std::move(LHS),
                                              std::move(RHS));
    }
}
