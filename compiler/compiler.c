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

    // puts("TOKEN_TYPE_IDENTIFIER\n");
    // for (int i = 0; i < vector_count(process->token_vec); i++)
    // {
    //     struct token tkn = *(struct token *)vector_at(process->token_vec, i);
    //     if (tkn.type == TOKEN_TYPE_IDENTIFIER)
    //         printf("%s ", tkn.sval);
    // }

    // printf("\n\nTOKEN_TYPE_KEYWORD\n");
    // for (int i = 0; i < vector_count(process->token_vec); i++)
    // {
    //     struct token tkn = *(struct token *)vector_at(process->token_vec, i);
    //     if (tkn.type == TOKEN_TYPE_KEYWORD)
    //         printf("%s ", tkn.sval);
    // }

    //  printf("\n\nTOKEN_TYPE_OPERATOR\n");
    // for (int i = 0; i < vector_count(process->token_vec); i++)
    // {
    //     struct token tkn = *(struct token *)vector_at(process->token_vec, i);
    //     if (tkn.type == TOKEN_TYPE_OPERATOR)
    //         printf("%s ", tkn.sval);
    // }

    //  printf("\n\nTOKEN_TYPE_SYMBOL\n");
    // for (int i = 0; i < vector_count(process->token_vec); i++)
    // {
    //     struct token tkn = *(struct token *)vector_at(process->token_vec, i);
    //     if (tkn.type == TOKEN_TYPE_SYMBOL)
    //         printf("%c ", tkn.cval);
    // }

    //  printf("\n\nTOKEN_TYPE_NUMBER\n");
    // for (int i = 0; i < vector_count(process->token_vec); i++)
    // {
    //     struct token tkn = *(struct token *)vector_at(process->token_vec, i);
    //     if (tkn.type == TOKEN_TYPE_NUMBER)
    //         printf("%d ", tkn.inum);
    // }

    //  printf("\n\nTOKEN_TYPE_STRING\n");
    // for (int i = 0; i < vector_count(process->token_vec); i++)
    // {
    //     struct token tkn = *(struct token *)vector_at(process->token_vec, i);
    //     if (tkn.type == TOKEN_TYPE_STRING)
    //         printf("%s ", tkn.sval);
    // }

    // printf("\n\nTOKEN_TYPE_COMMENT\n");
    // for (int i = 0; i < vector_count(process->token_vec); i++)
    // {
    //     struct token tkn = *(struct token *)vector_at(process->token_vec, i);
    //     if (tkn.type == TOKEN_TYPE_COMMENT)
    //         printf("%s ", tkn.sval);
    // }


    for (int i = 0; i < vector_count(process->token_vec); i++)
    {
        struct token tkn = *(struct token *)vector_at(process->token_vec, i);

        switch (tkn.type)
        {
        case TOKEN_TYPE_IDENTIFIER:
            printf("%s\n", tkn.sval);
            break;
        case TOKEN_TYPE_KEYWORD:
            printf("%s\n", tkn.sval);
            break;
        case TOKEN_TYPE_OPERATOR:
            printf("%s\n", tkn.sval);
            break;
        case TOKEN_TYPE_SYMBOL:
            printf("%c\n", tkn.cval);
            break;
        case TOKEN_TYPE_NUMBER:
            printf("%d\n", tkn.inum);
            break;
        case TOKEN_TYPE_STRING:
            printf("%s\n", tkn.sval);
            break;
        case TOKEN_TYPE_COMMENT:
            // we'll not pass the comment token to the parser
            break;
        case TOKEN_TYPE_NEWLINE:
            // we'll not pass the newline token to the parser
            break;
        default:
            break;
        }
    }

    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    putchar('\n');
    puts("\nScanning complete✅ ! 😘");
    printf("Scanner execution time: %f seconds\n", cpu_time_used);

    /* ************************** PARSING ******************************** */
    // to be implemented later
    /* ************************* CODE GENERATION **************************** */
    // to be implemented later

    return COMPILATION_COMPLETE;
}