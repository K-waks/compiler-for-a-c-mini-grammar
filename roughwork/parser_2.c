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

// Global variables for the scanner
FILE *fp;
char buffer[256];
int buffer_len;
enum Token current_token;

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

// Scanner function
void next_token()
{
    int c;
    buffer_len = 0;
    while ((c = fgetc(fp)) != EOF)
    {
        // TOKEN NEWLINE (also checks for whitespace)
        if (isspace(c))
        {
            if (c == '\n')
            {
                current_token = NEWLINE;
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
            if (strcmp(buffer, "int") == 0 || strcmp(buffer, "return") == 0 || strcmp(buffer, "if") == 0 || strcmp(buffer, "else") == 0 || strcmp(buffer, "while") == 0 || strcmp(buffer, "void") == 0)
            {
                current_token = KEYWORD;
            }
            else
            {
                current_token = IDENTIFIER;
            }
            return;
        }
        // TOKEN NUMBER
        else if (isdigit(c))
        {
            buffer[buffer_len++] = c;
            while ((c = fgetc(fp)) != EOF && isdigit(c))
            {
                buffer[buffer_len++] = c;
            }
            ungetc(c, fp);
            buffer[buffer_len] = '\0';
            current_token = NUMBER;
            return;
        }
        // TOKEN OPERATOR and TOKEN SYMBOL
        else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '<' || c == '>' || c == '!' || c == '&' || c == '|')
        {
            buffer[buffer_len++] = c;
            int lookahead = fgetc(fp);
            if ((c == '+' && lookahead == '+') || (c == '-' && lookahead == '-') || (c == '<' && lookahead == '=') || (c == '>' && lookahead == '=') || (c == '=' && lookahead == '=') || (c == '!' && lookahead == '=') || (c == '&' && lookahead == '&') || (c == '|' && lookahead == '|'))
            {
                buffer[buffer_len++] = lookahead;
                buffer[buffer_len] = '\0';
                current_token = OPERATOR;
            }
            else
            {
                ungetc(lookahead, fp);
                buffer[buffer_len] = '\0';
                current_token = SYMBOL;
            }
            return;
        }
        // TOKEN STRING
        else if (c == '"')
        {
            buffer[buffer_len++] = c;
            while ((c = fgetc(fp)) != EOF && c != '"')
            {
                buffer[buffer_len++] = c;
            }
            buffer[buffer_len++] = c;
            buffer[buffer_len] = '\0';
            current_token = STRING;
            return;
        }
        // TOKEN COMMENT
        else if (c == '/')
        {
            int lookahead = fgetc(fp);
            if (lookahead == '/')
            {
                while ((c = fgetc(fp)) != EOF && c != '\n')
                {
                    continue;
                }
                ungetc(c, fp);
                continue;
            }
            else if (lookahead == '*')
            {
                while ((c = fgetc(fp)) != EOF)
                {
                    int inner_lookahead = fgetc(fp);
                    if (c == '*' && inner_lookahead == '/')
                    {
                        break;
                    }
                    else
                    {
                        ungetc(inner_lookahead, fp);
                    }
                }
                continue;
            }
            else
            {
                ungetc(lookahead, fp);
                buffer[buffer_len++] = c;
                buffer[buffer_len] = '\0';
                current_token = OPERATOR;
                return;
            }
        }
        else
        {
            buffer[buffer_len++] = c;
            buffer[buffer_len] = '\0';
            current_token = SYMBOL;
            return;
        }
    }
    current_token = ENDOFFILE;
}




// Parser functions
void program()
{
    while (current_token != ENDOFFILE)
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
    // TODO: Implement variable_declaration()
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

int main()
{
    fp = fopen("program.c", "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error: Failed to open file");
        exit(EXIT_FAILURE);
    }

    next_token();
    program();

    fclose(fp);
    return 0;
}
