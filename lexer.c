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
            if (strcmp(buffer, "int") == 0 || strcmp(buffer, "return") == 0)
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
                exit(1);
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
                exit(1);
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
                buffer[buffer_len++] = c2;
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
            exit(1);
        }
    }
    return EOF;
}

int main()
{
    fp = fopen("program.c", "r");

    // -- Start the scanner timer
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    // -----------------------------------------------------

    
    if (fp == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    while ((current_token = get_token(fp, buffer)) != EOF)
    {
        switch (current_token)
        {
        case IDENTIFIER:
            printf("IDENTIFIER: %s\n", buffer);
            break;
        case KEYWORD:
            printf("KEYWORD: %s\n", buffer);
            break;
        case OPERATOR:
            printf("OPERATOR: %s\n", buffer);
            break;
        case SYMBOL:
            printf("SYMBOL: %s\n", buffer);
            break;
        case NUMBER:
            printf("NUMBER: %s\n", buffer);
            break;
        case STRING:
            printf("STRING: %s\n", buffer);
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
    
    

    // -----------------------------------------------------
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    putchar('\n');
    puts("\nScanning complete✅ !");
    printf("Scanner execution time: %f seconds\n", cpu_time_used);
    // -- Stop the timer
    
    memset(buffer, 0, sizeof(buffer));

    program();
    // printf("done\n\n");
    fclose(fp);
    return 0;
}

// Parser functions
enum Token next_token()
{
    return get_token(fp, buffer);
}

void program()
{
    while ((current_token = get_token(fp, buffer)) != EOF)
    {
        statement();
    }
}

void statement()
{
    if (current_token == KEYWORD && strcmp(buffer, "int") == 0)
    {
        variable_declaration();
    }
    else if (current_token == KEYWORD && strcmp(buffer, "void") == 0)
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
    if (current_token == KEYWORD && strcmp(buffer, "int") == 0)
    {
        // Consume the "int" keyword
        current_token = get_token(fp, buffer);

        // Expect an identifier
        if (current_token == IDENTIFIER)
        {
            // Do something with the identifier, like add it to a symbol table
            printf("Variable declared: %s\n", buffer);

            // Consume the identifier and semicolon
            current_token = get_token(fp, buffer);
            if (current_token != SYMBOL || buffer[0] != ';')
            {
                printf("Expected semicolon at end of variable declaration\n");
                exit(1);
            }
        }
        else
        {
            printf("Expected identifier after \"int\"\n");
            exit(1);
        }
    }
    else
    {
        printf("Expected \"int\" keyword for variable declaration\n");
        exit(1);
    }
}

void function_declaration()
{
    // TODO: Implement function_declaration()
}

void expression_statement()
{
    // TODO: Implement expression_statement()
}

void block()
{
    // TODO: Implement block()
}

void if_statement()
{
    // TODO: Implement if_statement()
}

void while_statement()
{
    // TODO: Implement while_statement()
}

void return_statement()
{
    // TODO: Implement return_statement()
}

void expression()
{
    // TODO: Implement expression()
}

void assignment()
{
    // TODO: Implement assignment()
}

void logical_or()
{
    // TODO: Implement logical_or()
}

void logical_and()
{
    // TODO: Implement logical_and()
}

void equality()
{
    // TODO: Implement equality()
}

void comparison()
{
    // TODO: Implement comparison()
}

void term()
{
    // TODO: Implement term()
}

void factor()
{
    // TODO: Implement factor()
}

void unary()
{
    // TODO: Implement unary()
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
