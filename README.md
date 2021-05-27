#Kaleidoscope

##Chapter 1 : Lexer

                  The language : Kaleidoscope.

                                 Some examples from LLVM tutorial :

```
#Compute the x'th fibonacci number.
    def fib(x) if x < 3 then 1 else fib(x - 1) + fib(x - 2)

#This expression will compute the 40th number.
                                                     fib(40)
```

```
#Using extern to call standard library functions
                          extern sin(arg);
extern cos(arg);
extern atan2(arg1 arg2);

atan2(sin(.4), cos(42))
```

The lexer just takes the sources code and tokenizes it.

## Chapter 2 : AST 

Expressions: Combination of one or more constants, variables, functions, and
operators and computes to produce another value.
Maybe any statement that produces a return value?

Statement: In an imperative programming language, a syntactic unit that
expresses some side effect.

Just useful to distinguish these since I see a lot of these distinctions in
code.

### Implementaton details

We want one object for each construct in the language. 

We use expressions and a prototype object (for function definations).

### Scaffolding

Expression Object
    - NumberExpr
        - Can directly store value
    - VariableExpr
        - Should only store name of variable since value is shared across 
        other objects corresponding to the same variable.
    - BinaryExpro
        - LHS op RHS. LHS, RHS -> Expressions
    - CallExpr
        - Store name of function and args passed. Args are also expressions.

    In a statically typed language with types other than double, this
    object would have a type field.

Prototype Object
    - Function
        - Name of function, Expression body of function
        - Functions are typed with number of arguements since 
        the only arguements can be doubles.

### Parser

Parser routines eats all the tokens corresponding to the productions and
returns the lexer buffer with the next token : Standard way for recursive
descent parsers.

Expression parsing:
    - Parse numbers
    - Parse parenthesis recursively (only used for precedense)
    - Parse identifiers (variable names, function calss with args)
    - Binary Expression parsing
        - Need to take care of precedence and associativity to remove ambiguity
