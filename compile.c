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
    // ENDOFFILE
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
char *tkn_type;

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
                continue;
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
        // else if (c == EOF)
        // {
        //     return ENDOFFILE;
        // }

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
    puts("\n**************************************\nStarting lexical analysis.....\n");

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
    tokens[num_tokens++].token_type = EOF;
    fclose(fp);

    end = clock(); // -- Stop the timer
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    putchar('\n');
    puts("\nLexical analysis complete✅ !");
    printf("Scanner execution time: %f seconds\n\n", cpu_time_used);

    // ************************************************ PARSING **********************************************
    clock_t start_parser, end_parser;
    double parser_cpu_time_used;
    start_parser = clock(); // -- Start the parser timer

    puts("\n**************************************\nStarting syntax analysis.....\n");
    current_token = tokens[pos].token_type;

    printf("PROGRAM->[");
    program();
    printf("]");

    end_parser = clock(); // -- Stop the parser timer
    parser_cpu_time_used = ((double)(end_parser - start_parser)) / CLOCKS_PER_SEC;
    putchar('\n');
    puts("\nParsing successful✅ !");
    printf("Parser execution time: %f seconds\n\n", parser_cpu_time_used);

    return 0;
}

// Parser functions
enum Token next_token()
{
    pos++;
    current_token = tokens[pos].token_type;

    switch (current_token)
    {
    case IDENTIFIER:
        tkn_type = "identifier";
        break;
    case KEYWORD:
        tkn_type = "keyword";
        break;
    case OPERATOR:
        tkn_type = "operator";
        break;
    case NUMBER:
        tkn_type = "number";
        break;
    case STRING:
        tkn_type = "string";
        break;
    case COMMENT:
        tkn_type = "comment";
        break;
    case NEWLINE:
        tkn_type = "newline";
        break;
    case SYMBOL:
        tkn_type = "symbol";
        break;

    default:
        break;
    }

    return current_token;
}

void program()
{
    printf("STATEMENT->[");
    while (current_token != EOF)
    {
        statement();
    }
    printf("]");
}

void statement()
{

    // printf("         \n\t|\n\t|\n\t|--->");
    // printf("         \n\t|\n\t|\n\t|--->");
    if (current_token == KEYWORD && strcmp(tokens[pos].value, "int") == 0)
    {
        printf("VARIABLE_DECLARATION->[");
        variable_declaration();
        printf("]");
    }
    else if (current_token == KEYWORD && strcmp(tokens[pos].value, "void") == 0)
    {
        printf("VARIABLE_DECLARATION->[");
        function_declaration();
        printf("]");
    }
    else
    {
        printf("EXPRESSION_STATEMENT->[");
        expression_statement();
        printf("]");
    }
}

void variable_declaration()
{

    next_token();
    printf("TOKEN %s->[%s", tkn_type, tokens[pos].value);
    printf("]");

    printf("IDENTIFIER->[");
    identifier();
    printf("]");
    if (current_token == SYMBOL && strcmp(tokens[pos].value, "=") == 0)
    {
        printf("ASSIGNMENT->[");
        assignment();
        printf("]");
    }
    printf("IDENTIFIER->[");
    while (current_token == SYMBOL && strcmp(tokens[pos].value, ",") == 0)
    {
        identifier();
        if (current_token == SYMBOL && strcmp(tokens[pos].value, "=") == 0)
        {

            printf("ASSIGNMENT->[");
            assignment();
            printf("]");
        }
    }
    printf("%s]", tokens[pos].value);
    if (current_token != SYMBOL || strcmp(tokens[pos].value, ";") == 1)
    {
        printf("\n\nExpected semicolon at the end of variable declaration ❌\n");
        exit(1);
    }
    // Consume semicolon
    next_token();
    printf("TOKEN %s->[%s", tkn_type, tokens[pos].value);
    printf("]");
    
}

void function_declaration()
{

    // Parse the function header
    if (current_token != KEYWORD || strcmp(tokens[pos].value, "void") != 0)
    {
        printf("Expected 'void' keyword for function declaration ❌\n");
        exit(EXIT_FAILURE);
    }
    next_token();
    printf("TOKEN %s->[%s", tkn_type, tokens[pos].value);
    printf("]");

    if (current_token != IDENTIFIER)
    {
        printf("Expected identifier after 'void' keyword for function declaration ❌\n");
        exit(EXIT_FAILURE);
    }
    next_token();
    printf("TOKEN %s->[%s", tkn_type, tokens[pos].value);
    printf("]");

    if (current_token != SYMBOL || strcmp(tokens[pos].value, "(") != 0)
    {
        printf("\n\nExpected '(' after function name for function declaration ❌\n");
        exit(EXIT_FAILURE);
    }
    next_token();
    printf("TOKEN %s->[%s", tkn_type, tokens[pos].value);
    printf("]");

    if (current_token != SYMBOL || strcmp(tokens[pos].value, ")") != 0)
    {
        printf("\n\nExpected ')' after function parameters for function declaration ❌\n");
        exit(EXIT_FAILURE);
    }
    next_token();
    printf("TOKEN %s->[%s", tkn_type, tokens[pos].value);
    printf("]");
    // Parse the function body
    block();
}

void expression_statement()
{
    printf("EXPRESSION->[");
    expression();
    printf("]");
    if (current_token != NEWLINE && current_token != EOF)
    {
        printf("Syntax error ❌: expected newline or end of file, found %s\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }
}

void block()
{
    if (current_token != SYMBOL || strcmp(tokens[pos].value, "{") != 0)
    {
        printf("\n\nSyntax error ❌: expected '{', found %s\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }
    printf("STATEMENT->[");
    while ((current_token = next_token()) != SYMBOL || strcmp(tokens[pos].value, "}") != 0)
    {
        statement();
    }
    printf("]");
}

void if_statement()
{
    if (current_token != KEYWORD || strcmp(tokens[pos].value, "if") != 0)
    {
        printf("Syntax error ❌: expected 'if', found %s\n", buffer);
        exit(EXIT_FAILURE);
    }
    printf("EXPRESSION->[");
    expression();
    printf("]");

    printf("BLOCK->[");
    block();
    printf("]");
    if ((current_token = next_token()) == KEYWORD && strcmp(tokens[pos].value, "else") == 0)
    {
        printf("BLOCK->[");
        block();
        printf("]");
    }
    else
    {
        // pos--;
        // current_token = tokens[pos].token_type;
        // ungetc(buffer[0], fp);
    }
}

void while_statement()
{
    if (current_token != KEYWORD || strcmp(tokens[pos].value, "while") != 0)
    {
        printf("Syntax error ❌: expected 'while', found %s\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }
    printf("EXPRESSION->[");
    expression();
    printf("]");

    printf("BLOCK->[");
    block();
    printf("]");
}

void return_statement()
{
    if (current_token != KEYWORD || strcmp(tokens[pos].value, "return") != 0)
    {
        printf("Syntax error ❌: expected 'return', found %s\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }
    printf("EXPRESSION->[");
    expression();
    printf("]");
    if (current_token != NEWLINE && current_token != EOF)
    {
        printf("Syntax error ❌: expected newline or end of file, found %s\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }
}

void expression()
{
    printf("LOGICAL_OR>[");
    logical_or();
    printf("]");
}

void assignment()
{
    printf("IDENTIFIER->[");
    identifier();
    printf("]");
    if (current_token != OPERATOR || strcmp(tokens[pos].value, "=") != 0)
    {
        printf("Syntax error ❌: expected '=', found %s\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }
    expression();
}

void logical_or()
{
    printf("LOGICAL_AND->[");
    logical_and();
    printf("]");
    while (current_token == OPERATOR && strcmp(tokens[pos].value, "|") == 0)
    {
        next_token();
        logical_and();
    }
}

void logical_and()
{

    equality();
    while (current_token == OPERATOR && strcmp(tokens[pos].value, "&") == 0)
    {
        current_token = get_token(fp, buffer);
        next_token();
        equality();
    }
}

void equality()
{
    printf("COMAPRISON->[");
    comparison();
    printf("]");
    printf("COMPARISON->[");
    while (current_token == OPERATOR && (strcmp(tokens[pos].value, "=") == 0 || strcmp(tokens[pos].value, "!") == 0))
    {
        next_token();

        comparison();
    }
    printf("]");
}

void comparison()
{
    printf("TERM->[");
    term();
    printf("]");
    printf("TERM->[");
    while (current_token == OPERATOR && (strcmp(tokens[pos].value, "<") == 0 || strcmp(tokens[pos].value, ">") == 0 ||
                                         strcmp(tokens[pos].value, "<=") == 0 || strcmp(tokens[pos].value, ">=") == 0))
    {
        next_token();
        printf("TOKEN %s->[%s", tkn_type, tokens[pos].value);
        printf("]");

        term();
    }
    printf("]");
}

// Parse a term, which is a product or division of factors.
void term()
{
    // Parse the first factor in the term
    printf("FACTOR->[");
    factor();
    printf("]");

    // Keep parsing factors while we have multiplication or division operators
    printf("FACTOR->[");
    while (current_token == OPERATOR && (strcmp(tokens[pos].value, "*") == 0 || strcmp(tokens[pos].value, "/") == 0))
    {
        // Save the current operator and get the next token
        enum Token operator_token = current_token;
        next_token();
        printf("TOKEN %s->[%s", tkn_type, tokens[pos].value);
        printf("]");

        // Parse the next factor in the term

        factor();

        // Generate code for the multiplication or division operation
        if (operator_token == OPERATOR && strcmp(tokens[pos].value, "*") == 0)
        {
            printf("MUL\n");
        }
        else if (operator_token == OPERATOR && strcmp(tokens[pos].value, "/") == 0)
        {
            printf("DIV\n");
        }
    }
    printf("]");
}

// Parse a factor, which is a number, variable, or parenthesized expression.
void factor()
{
    // Check for a unary operator (+ or -)
    if (current_token == OPERATOR && (strcmp(tokens[pos].value, "+") == 0 || strcmp(tokens[pos].value, "-") == 0))
    {
        // Save the current operator and get the next token
        enum Token operator_token = current_token;

        next_token();
        printf("TOKEN %s->[%s", tkn_type, tokens[pos].value);
        printf("]");
        // Parse the next factor
        printf("FACTOR->[");
        factor();
        printf("]");

        // Generate code for the unary operation
        if (operator_token == OPERATOR && strcmp(tokens[pos].value, "+") == 0)
        {
            // Do nothing for positive numbers
        }
        else if (operator_token == OPERATOR && strcmp(tokens[pos].value, "-") == 0)
        {
            printf("NEG\n");
        }
    }
    // Check for a parenthesized expression
    else if (current_token == SYMBOL && strcmp(tokens[pos].value, "(") == 0)
    {
        // Parse the expression inside the parentheses
        next_token();
        printf("TOKEN %s->[%s", tkn_type, tokens[pos].value);
        printf("]");
        expression();
        if (current_token != SYMBOL || strcmp(tokens[pos].value, ")") != 0)
        {
            printf("Syntax error: expected ')' after expression\n");
            exit(EXIT_FAILURE);
        }
        next_token();
        printf("TOKEN %s->[%s", tkn_type, tokens[pos].value);
        printf("]");
    } 
    // Otherwise, it must be a number or variable
    else
    {
        printf("PRIMARY->[");
        primary();
        printf("]");
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
        printf("PRIMARY->[");
        primary();
        printf("]");
    }
}

void primary()
{
    if (current_token == IDENTIFIER)
    {
        printf("IDENTIFIER->[");
        identifier();
        printf("]");
    }
    else if (current_token == NUMBER)
    {
        printf("NUMBER->[");
        number();
        printf("]");
    }
    else if (current_token == STRING)
    {
        printf("STRING->[");
        string();
        printf("]");
    }
    else if (current_token == SYMBOL && strcmp(tokens[pos].value, "(") == 0)
    {
        next_token();
        printf("TOKEN %s->[%s", tkn_type, tokens[pos].value);
        printf("]");
        expression();
        if (current_token != SYMBOL || strcmp(tokens[pos].value, ")") != 0)
        {
            fprintf(stderr, "Error: Missing ')' in expression.\n");
            exit(EXIT_FAILURE);
        }
        next_token();
        printf("TOKEN %s->[%s", tkn_type, tokens[pos].value);
        printf("]");
    }
    else if (current_token == SYMBOL && strcmp(tokens[pos].value, "}") == 0)
    {
        next_token();
        printf("TOKEN %s->[%s", tkn_type, tokens[pos].value);
        printf("]");
    }
    else
    {
        fprintf(stderr, "Error: Unexpected token '%s'.\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }
}

void identifier()
{
    if (current_token == IDENTIFIER)
    {
        next_token();
        printf("TOKEN %s->[%s", tkn_type, tokens[pos].value);
        printf("]");
    }
    else
    {   
        fprintf(stderr, "\n\n\nError: Expected identifier, but got '%s'.\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }
}

void number()
{
    if (current_token == NUMBER)
    {
        next_token();
        printf("TOKEN %s->[%s", tkn_type, tokens[pos].value);
        printf("]");
    }
    else
    {
        fprintf(stderr, "Error: Expected number, but got '%s'.\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }
}

void string()
{
    if (current_token == STRING)
    {
        next_token();
        printf("TOKEN %s->[%s", tkn_type, tokens[pos].value);
        printf("]");
    }
    else
    {
        fprintf(stderr, "Error: Expected string, but got '%s'.\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }
}
