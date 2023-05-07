#ifndef COMPILER_H
#define COMPILER_H

// token type enumerations
typedef enum
{
    KEYWORD = 31,
    IDENTIFIER = 32,
    NUMBER = 33,
    STRING = 34,
    OPERATOR = 35,
    SYMBOL = 36,

    // enum to handle unrecognized tokens and help in error handling
    INVALID_TOKEN = 37

} Token_Type;

// tokens will be in the form of structs so that their attributes can be filled and accessed.
typedef struct
{
    Token_Type type;
    union
    {
        char value[256];
        int ivalue;
        float fvalue;
    };
} Token;

// array of data type struct Token that stores a max of 1000 tokens (struct Token instances)
Token tokens[1000];

// enumerations for node type to aid in displaying them in their correct color in the terminal
typedef enum
{
    NON_TERMINAL,
    TERMINAL
} Node_Type;

// struct to store the parse tree
typedef struct node
{
    Node_Type type;
    char value[256];
    struct node *children[10];
    int num_children;
} Node;
 
// function prototypes for recursive descent functions in parser.c
int parser();
Node *declaration();
Node *variable_declaration();
Node *function_declaration();
Node *parameter_declaration();
Node *block();
Node *statement();
Node *if_statement();
Node *while_statement();
Node *return_statement();
Node *expression_statement();
Node *expression();
Node *assignment();
Node *logical_or();
Node *logical_and();
Node *equality();
Node *comparison();
Node *term();
Node *factor();
Node *unary();
Node *nested();
Node *primary();
Node *type_specifier();
Node *identifier();
Node *number();
Node *string();
Node *match(char *value);
Node *new_node(char *value, Node_Type type);
void add_child(Node *parent, Node *child);
void print_tree(Node *node, int depth);

#endif