#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

enum Token
{
    IDENTIFIER,
    KEYWORD,
    OPERATOR,
    NUMBER,
    STRING,
    COMMENT,
    NEWLINE,
    SYMBOL,
    ENDOFFILE
};

// Function prototypes for the parser
void program();
void statement();
void variable_declaration();
void function_declaration();
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
void identifier();
void number();
void string();

// Global Variables
FILE *fp;
char buffer[256];
enum Token current_token;

// structure to store the token and their values once scanned
struct Tkn
{
    enum Token token_type;
    char value[256];
};

struct Tkn tokens[1000]; // Maximum 1000 tokens
int num_tokens = 0;
int pos = 0;

enum Token get_token(FILE *fp, char *buffer)
{
    int c;
    int buffer_len = 0;
    while ((c = fgetc(fp)) != EOF)
    {
        // TOKEN NEWLINE (also checks for whitespace)
        if (isspace(c))
        {
            if (c == '\n')
            {
                return NEWLINE;
            }
            continue;
        }
        // TOKEN KEYWORD and TOKEN IDENTIFIER
        else if (isalpha(c) || c == '_')
        {
            buffer[buffer_len++] = c;
            while ((c = fgetc(fp)) != EOF && (isalnum(c) || c == '_'))
            {
                buffer[buffer_len++] = c;
            }
            ungetc(c, fp);
            buffer[buffer_len] = '\0';
            if (strcmp(buffer, "int") == 0 || strcmp(buffer, "return") == 0 || strcmp(buffer, "void") == 0 || strcmp(buffer, "char") == 0)
            {
                return KEYWORD;
            }
            else
            {
                return IDENTIFIER;
            }
        }
        // TOKEN NUMBER
        else if (isdigit(c))
        {
            buffer[buffer_len++] = c;
            while ((c = fgetc(fp)) != EOF && (isdigit(c) || c == '.'))
            {
                buffer[buffer_len++] = c;
            }
            if (isalpha(c) || c == '_')
            {
                printf("Invalid token ❌: %s%c\n", buffer, c);
                exit(EXIT_FAILURE);
            }
            ungetc(c, fp);
            buffer[buffer_len] = '\0';
            return NUMBER;
        }
        // TOKEN COMMENT
        else if (c == '/' && (c = fgetc(fp)) == '/')
        {
            while ((c = fgetc(fp)) != EOF && c != '\n')
                ;
            return COMMENT;
        }
        // TOKEN STRING
        else if (c == '"')
        {
            buffer[buffer_len++] = c;
            while ((c = fgetc(fp)) != EOF && c != '"')
            {
                buffer[buffer_len++] = c;
            }
            if (c == EOF)
            {
                printf("Invalid token ❌: %s\n", buffer);
                exit(EXIT_FAILURE);
            }
            buffer[buffer_len++] = c;
            buffer[buffer_len] = '\0';
            return STRING;
        }
        // TOKEN OPERATOR
        else if (c == '=' ||
                 c == '+' ||
                 c == '-' ||
                 c == '*' ||
                 c == '/' ||
                 c == '>' ||
                 c == '<' ||
                 c == '|' ||
                 c == '&' ||
                 c == '^' ||
                 c == '%' ||
                 c == '!')
        {
            buffer[buffer_len++] = c;
            int c2 = fgetc(fp);
            if (c2 == '=' ||               // dealing with ==, +=, -=, *=, /=, >=, <=, etc
                (c == '+' && c2 == '+') || // dealing with ++
                (c == '-' && c2 == '-') || // dealing with --
                (c == '|' && c2 == '|') || // dealing with ||
                (c == '&' && c2 == '&'))   // dealing with &&
            {
                buffer[buffer_len++] = c2; // +
            }
            else
            {
                ungetc(c2, fp);
            }
            buffer[buffer_len] = '\0';
            return OPERATOR;
        }
        // TOKEN SYMBOL
        else if (c == '{' ||
                 c == '}' ||
                 c == ':' ||
                 c == ';' ||
                 c == '#' ||
                 c == '\\' ||
                 c == '(' ||
                 c == ')' ||
                 c == '[' ||
                 c == ']' ||
                 c == ',')
        {
            buffer[buffer_len++] = c;
            buffer[buffer_len] = '\0';
            return SYMBOL;
        }
        // UNRECOGNIZED TOKEN
        else
        {
            printf("Invalid token ❌: %c\n\n", c);
            exit(EXIT_FAILURE);
        }
    }
    return EOF;
}

int main()
{
    // ************************************************ SCANNING **********************************************
    clock_t start, end;
    double cpu_time_used;
    start = clock(); // -- Start the scanner timer
    puts("\n-----------------------------------\nStarting lexical analysis.....\n");

    fp = fopen("program.c", "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    // saving each token to the struct array
    while ((current_token = get_token(fp, buffer)) != EOF)
    {
        struct Tkn current_tkn;
        current_tkn.token_type = current_token;
        strcpy(current_tkn.value, buffer);
        tokens[num_tokens++] = current_tkn;
    }
    // tokens[num_tokens++].token_type = EOF;

    // printing to the terminal
    for (int i = 0; i < num_tokens; i++)
    {
        switch (tokens[i].token_type)
        {
        case IDENTIFIER:
            printf("IDENTIFIER: %s\n", tokens[i].value);
            break;
        case KEYWORD:
            printf("KEYWORD: %s\n", tokens[i].value);
            break;
        case OPERATOR:
            printf("OPERATOR: %s\n", tokens[i].value);
            break;
        case SYMBOL:
            printf("SYMBOL: %s\n", tokens[i].value);
            break;
        case NUMBER:
            printf("NUMBER: %s\n", tokens[i].value);
            break;
        case STRING:
            printf("STRING: %s\n", tokens[i].value);
            break;
        case COMMENT:
            printf("COMMENT\n");
            break;
        case NEWLINE:
            printf("NEWLINE\n");
            break;
        default:
            printf("Unknown token\n");
        }
    }
    fclose(fp);

    end = clock(); // -- Stop the timer
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    putchar('\n');
    puts("-----------------------------------\nLexical analysis complete✅ !");
    printf("Scanner execution time: %f seconds\n\n", cpu_time_used);

    // ************************************************ PARSING **********************************************
    clock_t start_parser, end_parser;
    double parser_cpu_time_used;
    start_parser = clock(); // -- Start the parser timer

    puts("\n-----------------------------------\nStarting syntax analysis.....\n");
    current_token = tokens[pos].token_type;
    program();

    end_parser = clock(); // -- Stop the parser timer
    parser_cpu_time_used = ((double)(end_parser - start_parser)) / CLOCKS_PER_SEC;
    putchar('\n');
    puts("-----------------------------------\nSyntax analysis complete✅ !");
    printf("Parser execution time: %f seconds\n\n", parser_cpu_time_used);

    return 0;
}

// Parser functions
enum Token next_token()
{
    pos++;
    return tokens[pos].token_type;
}

void program()
{
    while (current_token != EOF)
    {
        statement();
    }
}

void statement()
{
    if (current_token == KEYWORD && strcmp(tokens[pos].value, "int") == 0)
    {
        variable_declaration();
    }
    else if (current_token == KEYWORD && strcmp(tokens[pos].value, "void") == 0)
    {
        function_declaration();
    }
    else
    {
        expression_statement();
    }
}

void variable_declaration()
{
    if (current_token == KEYWORD && strcmp(tokens[pos].value, "int") == 0)
    {
        pos++;
        current_token = tokens[pos].token_type; // Consume the "int" keyword
        if (current_token == IDENTIFIER)        // Expect an identifier
        {
            printf("Variable declared: %s\n", tokens[pos].value); // Do something with the identifier, like add it to a symbol table
            pos++;
            current_token = tokens[pos].token_type; // Consume the identifier and semicolon
            if (current_token != SYMBOL || strcmp(tokens[pos].value, ";") == 1)
            {
                printf("Expected semicolon at end of variable declaration\n");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            printf("Expected identifier after \"int\"\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        printf("Expected \"int\" keyword for variable declaration\n");
        exit(EXIT_FAILURE);
    }
}

void function_declaration()
{
    // Parse the function header
    if (current_token != KEYWORD || strcmp(buffer, "int") != 0)
    {
        printf("Expected 'int' keyword for function declaration ❌\n");
        exit(EXIT_FAILURE);
    }
    get_token(fp, buffer); // Move to the next token
    if (current_token != IDENTIFIER)
    {
        printf("Expected identifier after 'int' keyword for function declaration ❌\n");
        exit(EXIT_FAILURE);
    }
    get_token(fp, buffer); // Move to the next token
    if (current_token != SYMBOL || strcmp(buffer, "(") != 0)
    {
        printf("Expected '(' after function name for function declaration ❌\n");
        exit(EXIT_FAILURE);
    }
    // Parse the function parameters
    while ((current_token = get_token(fp, buffer)) == KEYWORD || current_token == IDENTIFIER)
    {
        // TODO: handle function parameters
    }
    if (current_token != SYMBOL || strcmp(buffer, ")") != 0)
    {
        printf("Expected ')' after function parameters for function declaration ❌\n");
        exit(EXIT_FAILURE);
    }
    // Parse the function body
    block();
}

void expression_statement()
{
    expression();
    if (current_token != NEWLINE && current_token != ENDOFFILE)
    {
        printf("Syntax error ❌: expected newline or end of file, found %s\n", buffer);
        exit(EXIT_FAILURE);
    }
}

void block()
{
    if (current_token != SYMBOL || buffer[0] != '{')
    {
        printf("Syntax error ❌: expected '{', found %s\n", buffer);
        exit(EXIT_FAILURE);
    }
    while ((current_token = get_token(fp, buffer)) != SYMBOL || buffer[0] != '}')
    {
        statement();
    }
}

void if_statement()
{
    if (current_token != KEYWORD || strcmp(buffer, "if") != 0)
    {
        printf("Syntax error ❌: expected 'if', found %s\n", buffer);
        exit(EXIT_FAILURE);
    }
    expression();
    block();
    if ((current_token = get_token(fp, buffer)) == KEYWORD && strcmp(buffer, "else") == 0)
    {
        block();
    }
    else
    {
        ungetc(buffer[0], fp);
    }
}

void while_statement()
{
    if (current_token != KEYWORD || strcmp(buffer, "while") != 0)
    {
        printf("Syntax error ❌: expected 'while', found %s\n", buffer);
        exit(EXIT_FAILURE);
    }
    expression();
    block();
}

void return_statement()
{
    if (current_token != KEYWORD || strcmp(buffer, "return") != 0)
    {
        printf("Syntax error ❌: expected 'return', found %s\n", buffer);
        exit(EXIT_FAILURE);
    }
    expression();
    if (current_token != NEWLINE && current_token != ENDOFFILE)
    {
        printf("Syntax error ❌: expected newline or end of file, found %s\n", buffer);
        exit(EXIT_FAILURE);
    }
}

void expression()
{
    logical_or();
}

void assignment()
{
    identifier();
    if (current_token != OPERATOR || buffer[0] != '=')
    {
        printf("Syntax error ❌: expected '=', found %s\n", buffer);
        exit(EXIT_FAILURE);
    }
    expression();
}

void logical_or()
{
    logical_and();
    while (current_token == OPERATOR && buffer[0] == '|')
    {
        current_token = get_token(fp, buffer);
        logical_and();
    }
}

void logical_and()
{
    equality();
    while (current_token == OPERATOR && buffer[0] == '&')
    {
        current_token = get_token(fp, buffer);
        equality();
    }
}

void equality()
{
    comparison();
    while (current_token == OPERATOR && (buffer[0] == '=' || buffer[0] == '!'))
    {
        current_token = get_token(fp, buffer);
        comparison();
    }
}

void comparison()
{
    term();
    while (current_token == OPERATOR && (buffer[0] == '<' || buffer[0] == '>' ||
                                         strcmp(buffer, "<=") == 0 || strcmp(buffer, ">=") == 0))
    {
        current_token = get_token(fp, buffer);
        term();
    }
}

// Parse a term, which is a product or division of factors.
void term()
{
    // Parse the first factor in the term
    factor();

    // Keep parsing factors while we have multiplication or division operators
    while (current_token == OPERATOR && (strcmp(buffer, "*") == 0 || strcmp(buffer, "/") == 0))
    {
        // Save the current operator and get the next token
        enum Token operator_token = current_token;
        get_token(fp, buffer);

        // Parse the next factor in the term
        factor();

        // Generate code for the multiplication or division operation
        if (operator_token == OPERATOR && strcmp(buffer, "*") == 0)
        {
            printf("MUL\n");
        }
        else if (operator_token == OPERATOR && strcmp(buffer, "/") == 0)
        {
            printf("DIV\n");
        }
    }
}

// Parse a factor, which is a number, variable, or parenthesized expression.
void factor()
{
    // Check for a unary operator (+ or -)
    if (current_token == OPERATOR && (strcmp(buffer, "+") == 0 || strcmp(buffer, "-") == 0))
    {
        // Save the current operator and get the next token
        enum Token operator_token = current_token;
        get_token(fp, buffer);

        // Parse the next factor
        factor();

        // Generate code for the unary operation
        if (operator_token == OPERATOR && strcmp(buffer, "+") == 0)
        {
            // Do nothing for positive numbers
        }
        else if (operator_token == OPERATOR && strcmp(buffer, "-") == 0)
        {
            printf("NEG\n");
        }
    }
    // Check for a parenthesized expression
    else if (current_token == SYMBOL && strcmp(buffer, "(") == 0)
    {
        // Parse the expression inside the parentheses
        get_token(fp, buffer);
        expression();
        if (current_token != SYMBOL || strcmp(buffer, ")") != 0)
        {
            printf("Syntax error: expected ')' after expression\n");
            exit(EXIT_FAILURE);
        }
        get_token(fp, buffer);
    }
    // Otherwise, it must be a number or variable
    else
    {
        primary();
    }
}

// Parse a unary operator (+ or -) or a primary (number or variable).
void unary()
{
    // Check for a unary operator (+ or -)
    if (current_token == OPERATOR && (strcmp(buffer, "+") == 0 || strcmp(buffer, "-") == 0))
    {
        // Save the current operator and get the next token
        enum Token operator_token = current_token;
        get_token(fp, buffer);

        // Parse the next expression
        unary();

        // Generate code for the unary operation
        if (operator_token == OPERATOR && strcmp(buffer, "+") == 0)
        {
            // Do nothing for positive numbers
        }
        else if (operator_token == OPERATOR && strcmp(buffer, "-") == 0)
        {
            printf("NEG\n");
        }
    }
    // Otherwise, it must be a primary (number or variable)
    else
    {
        primary();
    }
}

void primary()
{
    if (current_token == IDENTIFIER)
    {
        identifier();
    }
    else if (current_token == NUMBER)
    {
        number();
    }
    else if (current_token == STRING)
    {
        string();
    }
    else if (current_token == SYMBOL && buffer[0] == '(')
    {
        next_token();
        expression();
        if (current_token != SYMBOL || buffer[0] != ')')
        {
            fprintf(stderr, "Error: Missing ')' in expression.\n");
            exit(EXIT_FAILURE);
        }
        next_token();
    }
    else
    {
        fprintf(stderr, "Error: Unexpected token '%s'.\n", buffer);
        exit(EXIT_FAILURE);
    }
}

void identifier()
{
    if (current_token == IDENTIFIER)
    {
        next_token();
    }
    else
    {
        fprintf(stderr, "Error: Expected identifier, but got '%s'.\n", buffer);
        exit(EXIT_FAILURE);
    }
}

void number()
{
    if (current_token == NUMBER)
    {
        next_token();
    }
    else
    {
        fprintf(stderr, "Error: Expected number, but got '%s'.\n", buffer);
        exit(EXIT_FAILURE);
    }
}

void string()
{
    if (current_token == STRING)
    {
        next_token();
    }
    else
    {
        fprintf(stderr, "Error: Expected string, but got '%s'.\n", buffer);
        exit(EXIT_FAILURE);
    }
}
