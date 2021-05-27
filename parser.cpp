#include "parser.h"

/**
 * Scafolding
 **/

/* Expressions */
class ExprAST {
   public:
    virtual ~ExprAST() {}

    virtual std::string pprint() {
        return std::string("Base ExprAST Class\n");
    };
};

class NumberExprAST : public ExprAST {
    double Val;

   public:
    NumberExprAST(double Val) : Val(Val) {}

    std::string pprint() { return std::string(std::to_string(Val)); }
};

class VariableExprAST : public ExprAST {
    std::string Name;

   public:
    VariableExprAST(std::string &Name) : Name(Name) {}

    std::string pprint() { return std::string(Name); }
};

class BinaryExprAST : public ExprAST {
    char op;
    std::unique_ptr<ExprAST> LHS, RHS;

   public:
    BinaryExprAST(char op, std::unique_ptr<ExprAST> LHS,
                  std::unique_ptr<ExprAST> RHS)
        : op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

    std::string pprint() {
        return "(" + LHS->pprint() + " " + std::string(1, op) + " " +
               RHS->pprint() + ")";
    }
};

class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;

   public:
    CallExprAST(const std::string &Callee,
                std::vector<std::unique_ptr<ExprAST>> Args)
        : Callee(Callee), Args(std::move(Args)) {}

    std::string pprint() {
        std::string ret = "(call: " + Callee;
        for (auto &it : Args) {
            ret += " " + it->pprint();
        }
        ret += ")";
        return ret;
    }
};

/* Prototypes */
class PrototypeAST {
    std::string Name;
    std::vector<std::string> Args;

   public:
    PrototypeAST(const std::string &name, std::vector<std::string> Args)
        : Name(name), Args(std::move(Args)) {}

    std::string pprint() {
        std::string ret = "prototype: " + Name;
        for (auto &it : Args) {
            ret += " " + it;
        }
        return ret;
    }
};

class FunctionAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<ExprAST> Body;

   public:
    FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                std::unique_ptr<ExprAST> Body)
        : Proto(std::move(Proto)), Body(std::move(Body)) {}

    std::string pprint() {
        return "function def " + Proto->pprint() + "\n" + Body->pprint();
    }
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

/* Expression parsing */
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

/* Function Definition Parsing */
static std::unique_ptr<PrototypeAST> parsePrototype() {
    if (CurTok != tokIdentifier) {
        return LogErrorP("Expected function name in prototype");
    }

    auto fnName = TokenIdentifier;
    getToken();

    if (CurTok != '(') {
        return LogErrorP("Expected '(' in prototype");
    }

    std::vector<std::string> argNames;
    while (getToken() == tokIdentifier) {
        argNames.push_back(TokenIdentifier);
    }

    if (CurTok != ')') {
        return LogErrorP("Expected ')' in prototype");
    }

    getToken();

    return std::make_unique<PrototypeAST>(fnName, std::move(argNames));
}

static std::unique_ptr<FunctionAST> parseDefinition() {
    getToken();
    auto proto = parsePrototype();
    if (!proto) {
        return nullptr;
    }

    if (auto expr = parseExpr()) {
        return std::make_unique<FunctionAST>(std::move(proto), std::move(expr));
    }
    return nullptr;
}

/* Extern standard library calls */
static std::unique_ptr<PrototypeAST> parseExtern() {
    getToken();
    return parsePrototype();
}

/* Create an anonymous function for top level expressions */
static std::unique_ptr<FunctionAST> parseTopLevelExpr() {
    if (auto expr = parseExpr()) {
        auto proto =
            std::make_unique<PrototypeAST>("", std::vector<std::string>());
        return std::make_unique<FunctionAST>(std::move(proto), std::move(expr));
    }

    return nullptr;
}

/* Top ::= Defintion | External | Expression | ';' */
void parse(bool interpret) {
    if (interpret) {
        fprintf(stderr, "kaliedoscope> ");
    }

    getToken();

    while (true) {
        switch (CurTok) {
            case tokEOF:
                return;
            case ';':
                getToken();
                break;
            case tokDef:
                std::cout << parseDefinition()->pprint() << "\n";
                break;
            case tokExtern:
                std::cout << parseExtern()->pprint() << "\n";
                break;
            default:
                std::cout << parseTopLevelExpr()->pprint() << "\n";
                break;
        }

        if (interpret and CurTok != tokEOF) {
            fprintf(stderr, "kaliedoscope> ");
        }
    }
}
