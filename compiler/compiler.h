#ifndef C_COMPILER_H
#define C_COMPILER_H

// token type enumerations
enum Token_Type
{
    KEYWORD = 31,
    IDENTIFIER = 32,
    NUMBER = 33,
    STRING = 34,
    OPERATOR = 35,
    SYMBOL = 36,

    // enum to handle unrecognized tokens and help in error handling
    INVALID_TOKEN = 37
};

// tokens will be in the form of structs so that their attributes can be filled and accessed.
struct Token
{
    enum Token_Type type;
    union
    {
        char value[256];
        int ivalue;
        float fvalue;
    };
};

// array of data type struct Token that stores a max of 1000 tokens (struct Token instances)
struct Token tokens[1000];

// function prototypes for the parser
void program();
void declaration();
void variable_declaration();
void function_declaration();
void parameter_declaration();
void statement();
void expression_statement();
void block();
void if_statement();
void while_statement();
void return_statement();
void expression();
void assignment();
void logical_or();
void logical_and();
void equality();
void comparison();
void term();
void factor();
void unary();
void primary();
void type_specifier();
void identifier();
void number();
void string();
void match(char *value);

#endif