#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "compile.h"

enum Token get_token(FILE *fp, char *buffer)
{
    int c;
    int buffer_len = 0;
    while ((c = fgetc(fp)) != EOF)
    {
        if (isspace(c))
        {
            if (c == '\n')
            {
                return NEWLINE;
            }
            continue;
        }
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
        else if (c == '/' && (c = fgetc(fp)) == '/')
        {
            while ((c = fgetc(fp)) != EOF && c != '\n')
                ;
            return COMMENT;
        }
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
        else if (c == '{' ||
                 c == '}' ||
                 c == ':' ||
                 c == ';' ||
                 c == '#' ||
                 c == '\\' ||
                 c == '(' ||
                 c == ')' ||
                 c == ']' ||
                 c == ',')
        {
            buffer[buffer_len++] = c;
            buffer[buffer_len] = '\0';
            return SYMBOL;
        }

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
    // Start the timer
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    // **************************************

    FILE *fp = fopen("program.c", "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    char buffer[256];
    enum Token token;

    while ((token = get_token(fp, buffer)) != EOF)
    {
        switch (token)
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

    fclose(fp);

    // **************************************
    // Stop the timer
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    putchar('\n');
    puts("\nScanning complete✅ !");
    printf("Scanner execution time: %f seconds\n", cpu_time_used);

    return 0;
}