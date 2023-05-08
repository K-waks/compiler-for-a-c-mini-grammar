#include <time.h>
#include "compiler/tac.c"

int main()
{
    int r;
    clock_t start, end;
    double cpu_time_used;

    /* ******************************** LEXER ********************************* */

    puts("\n🕑 Starting Lexical Analysis\n..................................");
    start = clock();

    lexer();

    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\n✅ Lexical analysis successful. Execution time: %f seconds\n\n", cpu_time_used);

    /* ******************************** PARSER ********************************* */

    puts("\n🕑 Starting Parsing\n..................................");
    start = clock();

    parser();

    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\n✅ Parsing successful. Execution time: %f seconds\n\n", cpu_time_used);

   /* ******************************** CODE GENERATOR ********************************* */

    generate_code(root_node); // call the generator()

    return 0;
}