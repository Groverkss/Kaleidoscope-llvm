# Kaleidoscope

## Chapter 1

The language: Kaleidoscope.

Some examples from LLVM tutorial:

```
# Compute the x'th fibonacci number.
def fib(x)
  if x < 3 then
    1
  else
    fib(x-1)+fib(x-2)

# This expression will compute the 40th number.
fib(40)
```

```
# Using extern to call standard library functions
extern sin(arg);
extern cos(arg);
extern atan2(arg1 arg2);

atan2(sin(.4), cos(42))
```

Implemented the lexer to tokenize the language.
