#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "compile.h"

enum NonTerminal
{
    PROGRAM,
    STATEMENT,
    ASSIGNMENT,
    EXPRESSION,
    TERM,
    FACTOR,
    PRIMARY
};

int store;
enum Token current_token;
char token_buffer[256];

void error(const char *message)
{
    fprintf(stderr, "Parse error: %s\n", message);
    exit(1);
}

void match(enum Token expected_token)
{
    if (current_token == expected_token)
    {
        scanf("%d", &store);
        current_token = (enum Token)store;

        // current_token = get_token(stdin, token_buffer);
    }
    else
    {
        error("unexpected token");
    }
}

void program()
{
    statement();
    while (current_token != EOF)
    {
        match(NEWLINE);
        statement();
    }
}

void statement()
{
    if (current_token == IDENTIFIER)
    {
        assignment();
    }
    else
    {
        error("invalid statement");
    }
}

void assignment()
{
    match(IDENTIFIER);
    match('=');
    expression();
}

void expression()
{
    term();
    while (current_token == '+' || current_token == '-')
    {
        match(current_token);
        term();
    }
}

void term()
{
    factor();
    while (current_token == '*' || current_token == '/')
    {
        match(current_token);
        factor();
    }
}

void factor()
{
    primary();
    while (current_token == '^')
    {
        match(current_token);
        primary();
    }
}

void primary()
{
    if (current_token == NUMBER || current_token == STRING)
    {
        match(current_token);
    }
    else if (current_token == IDENTIFIER)
    {
        match(IDENTIFIER);
    }
    else if (current_token == '(')
    {
        match('(');
        expression();
        match(')');
    }
    else
    {
        error("invalid primary expression");
    }
}

int main()
{
    scanf("%d", &store);
    current_token = (enum Token)store;

    // current_token = get_token(stdin, token_buffer);

    program();
    return 0;
}
