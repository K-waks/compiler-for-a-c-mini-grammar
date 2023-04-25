#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include "compiler/compiler.h"
#include "compiler/lexer.c"
#include "compiler/parser.c"


int main()
{
    int result;

    // ********************************* LEXER ****************************************
    puts("\n🕑 Starting Lexical Analysis\n..................................");
    clock_t start_scanner, end_scanner;
    double scanner_cpu_time_used;
    start_scanner = clock(); // start scanner timer

    result = lexer(); // call the lexer

    end_scanner = clock(); // -- Stop the timer
    scanner_cpu_time_used = ((double)(end_scanner - start_scanner)) / CLOCKS_PER_SEC;

    switch (result)
    {
    case EXIT_FAILURE:
        printf("\n🚫 Lexer FAILURE!\n\n");
        break;
    case EXIT_SUCCESS:
        printf("\n✅ Lexical analysis successful. Execution time: %f seconds\n\n", scanner_cpu_time_used);
        break;
    default:
        printf("\n⛔ Lexer could not be called.\n\n");
        break;
    }

    // ********************************* PARSER **************************************
    if (result == EXIT_FAILURE) // if the scanner failed the parser will not be called
    {
        return 1;
    }
    
    puts("\n🕑 Starting Parsing\n..................................");
    clock_t start_parser, end_parser;
    double parser_cpu_time_used;
    start_parser = clock(); // start parser timer

    result = parser(); // call the parser

    end_parser = clock(); // -- Stop the timer
    parser_cpu_time_used = ((double)(end_parser - start_parser)) / CLOCKS_PER_SEC;

    switch (result)
    {
    case EXIT_FAILURE:
        printf("\n🚫 Parser FAILURE!\n\n");
        break;
    case EXIT_SUCCESS:
        printf("\n\n✅ Parsing successful. Execution time: %f seconds\n\n", parser_cpu_time_used);
        break;
    default:
        printf("\n⛔ Parser could not be called.\n\n");
        break;
    }

    return 0;
}