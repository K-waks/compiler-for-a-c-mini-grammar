#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include "compiler.h"
#include "helpers/vector.h"

struct lex_process_functions compiler_lex_functions = {
    .next_char = compile_process_next_char,
    .peek_char = compile_process_peek_char,
    .push_char = compile_process_push_char};

void compiler_error(struct compile_process *compiler, const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, " on line %i, col %i in file %s\n", compiler->pos.line, compiler->pos.col, compiler->pos.filename);
    exit(-1);
}

void compiler_warning(struct compile_process *compiler, const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, " on line %i, col %i in file %s\n", compiler->pos.line, compiler->pos.col, compiler->pos.filename);
}

int compile_c_file(const char *sourcefile, const char *outputfile, int flags)
{
    struct compile_process *process = compile_process_create(sourcefile, outputfile, flags);

    if (!process)
        return COMPILATION_FAILURE;

    /* ***************************** SCANNING ******************************** */
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    struct lex_process *lex_process = lex_process_create(process, &compiler_lex_functions, NULL);
    if (!lex_process)
    {
        puts("Scanning failed with errors❌!");
        return COMPILATION_FAILURE;
    }

    if (lex(lex_process) != LEXICAL_ANALYSIS_ALL_OK)
    {
        puts("Scanning failed with errors❌!");
        return COMPILATION_FAILURE;
    }

    // token list
    process->token_vec = lex_process->token_vec;

    for (int i = 0; i < vector_count(process->token_vec); i++)
    {
        struct token tkn = *(struct token *)vector_at(process->token_vec, i);

        if (tkn.type == TOKEN_TYPE_IDENTIFIER)
            printf("TOKEN_TYPE_IDENTIFIER:       %s\n", tkn.sval);
        else if (tkn.type == TOKEN_TYPE_KEYWORD)
        {
            printf("TOKEN_TYPE_KEYWORD:          %s\n", tkn.sval);
        }
        else if (tkn.type == TOKEN_TYPE_OPERATOR)
        {
            printf("TOKEN_TYPE_OPERATOR:         %s\n", tkn.sval);
        }
        else if (tkn.type == TOKEN_TYPE_SYMBOL)
        {
            printf("TOKEN_TYPE_SYMBOL:           %c\n", tkn.cval);
        }
        else if (tkn.type == TOKEN_TYPE_NUMBER && tkn.num.type == NUMBER_TYPE_NORMAL)
        {
            printf("TOKEN_TYPE_NUMBER:           %lld\n", tkn.llnum);
        }

        else if (tkn.type == TOKEN_TYPE_NUMBER && tkn.num.type == NUMBER_TYPE_FLOAT)
        {
            printf("TOKEN_TYPE_NUMBER:           %f\n", tkn.fnum);
        }

        else if (tkn.type == TOKEN_TYPE_STRING)
        {
            printf("TOKEN_TYPE_STRING:           %s\n", tkn.sval);
        }
        else if (tkn.type == TOKEN_TYPE_COMMENT)
        {
            printf("TOKEN_TYPE_COMMENT:          %s\n", tkn.sval);
        }
        // else if (tkn.type == TOKEN_TYPE_NEWLINE)
        // {
        //     printf("TOKEN_TYPE_NEWLINE:          %c\n", tkn.cval);
        // }
        // else
        // {
        //     printf("Error! Unidentified token!\n");
        // }
    }

    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    putchar('\n');
    puts("\nScanning complete✅ !");
    printf("Scanner execution time: %f seconds\n", cpu_time_used);

    /* ************************** PARSING ******************************** */
    
    /* ************************* CODE GENERATION **************************** */
    

    return COMPILATION_COMPLETE;
}