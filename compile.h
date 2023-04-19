#ifndef C_COMPILER_H
#define C_COMPILER_H

#include <stdio.h>

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


enum Token get_token(FILE *fp, char *buffer);

void error(const char *message);
void match(enum Token expected_token);
void program();
void statement();
void assignment();
void expression();
void term();
void factor();
void primary();

#endif