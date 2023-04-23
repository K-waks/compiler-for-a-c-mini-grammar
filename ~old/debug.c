#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// token type enumerations
enum Token_Type
{
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    STRING,
    OPERATOR,
    SYMBOL,
};

// function prototypes for the parser
enum Token_Type next_token(char *location);
void parser();
void statement();
void declaration();
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

// file descriptor variable declared and buffer of dynamic size to hold the read characters
FILE *fp;
char buffer[BUFSIZ];

// tokens will be in the form of structs so that their attributes can be filled and accessed.
struct Token
{
    enum Token_Type type;
    union
    {
        char value[256];
        int inum;
        float fnum;
    };
    // char value[256];
};

// array of data type struct Token that stores a max of 1000 tokens (struct Token instances)
struct Token tokens[1000];

// other global variables
enum Token_Type token_type;
int token_count = 0;
int pos = 0;
char *tkn_type;

// scanner function of datatype enum Token_Type that reads contents of the a c file and generates token output
// the function returns an enumeration value
enum Token_Type scanner(FILE *fp, char *buffer)
{
    int c;
    int buffer_len = 0;
    while ((c = fgetc(fp)) != EOF)
    {
        // newline and whitespace are ignored
        if (isspace(c))
        {
            if (c == '\n')
            {
                continue;
            }
            continue;
        }
        // comments are also ignored
        else if (c == '/' && (c = fgetc(fp)) == '/')
        {
            while ((c = fgetc(fp)) != EOF && c != '\n')
                ;
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
            if (strcmp(buffer, "void") == 0 || strcmp(buffer, "int") == 0 || strcmp(buffer, "char") == 0 || strcmp(buffer, "float") == 0 || strcmp(buffer, "if") == 0 || strcmp(buffer, "else") == 0 || strcmp(buffer, "while") == 0 || strcmp(buffer, "for") == 0 || strcmp(buffer, "break") == 0 || strcmp(buffer, "continue") == 0 || strcmp(buffer, "switch") == 0 || strcmp(buffer, "case") == 0 || strcmp(buffer, "return") == 0)
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
                printf("\n\n❌ ERROR! Invalid token: %s%c\n\n", buffer, c);
                exit(EXIT_FAILURE);
            }
            ungetc(c, fp);
            buffer[buffer_len] = '\0';
            return NUMBER;
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
                printf("\n\n❌ ERROR! Invalid token: %s\n\n", buffer);
                exit(EXIT_FAILURE);
            }
            buffer[buffer_len++] = c;
            buffer[buffer_len] = '\0';
            return STRING;
        }
        // TOKEN OPERATOR
        else if (c == '=' || c == '+' || c == '-' || c == '*' || c == '/' || c == '>' || c == '<' || c == '|' || c == '&' || c == '^' || c == '%' || c == '!')
        {
            buffer[buffer_len++] = c;
            int c2 = fgetc(fp);
            if (c2 == '=' || (c == '+' && c2 == '+') || (c == '-' && c2 == '-') || (c == '|' && c2 == '|') || (c == '&' && c2 == '&'))
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
        else if (c == '{' || c == '}' || c == ':' || c == ';' || c == '#' || c == '\\' || c == '(' || c == ')' || c == '[' || c == ']' || c == ',')
        {
            buffer[buffer_len++] = c;
            buffer[buffer_len] = '\0';
            return SYMBOL;
        }
        // UNRECOGNIZED TOKEN
        else
        {
            printf("\n\n❌ ERROR! Invalid token: %c\n\n", c);
            exit(EXIT_FAILURE);
        }
    }
    return EOF;
}

int main()
{
    // ************************************************ SCANNING **********************************************

    // start scanner timer
    puts("\n🕑 Starting Scanning\n..................................");
    clock_t start_scanner, end_scanner;
    double scanner_cpu_time_used;
    start_scanner = clock();

    // opening the file for scanning
    fp = fopen("sample_program.c", "r");
    if (fp == NULL)
    {
        printf("\n\n❌ ERROR! Can't open file for scanning.\n\n");
        exit(EXIT_FAILURE);
    }
    // saving each token to the struct array
    while ((token_type = scanner(fp, buffer)) != EOF)
    {
        struct Token current_tkn;
        current_tkn.type = token_type;
        strcpy(current_tkn.value, buffer);
        tokens[token_count++] = current_tkn;
    }
    // adding the terminating symbol to the end of the array that will be used to tell the parser that we've reached the end of parsing
    struct Token terminator;
    terminator.type = EOF;
    strcpy(terminator.value, "$");
    tokens[token_count++] = terminator;

    // printing to the terminal
    for (int i = 0; i < token_count; i++)
    {
        switch (tokens[i].type)
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
        case EOF:
            printf("Parser terminator symbol: %s\n", tokens[i].value);
            break;
        default:
            puts("Unknown token\n");
        }
    }
    // closing the file
    fclose(fp);

    // stop scanner timer
    end_scanner = clock(); // -- Stop the timer
    scanner_cpu_time_used = ((double)(end_scanner - start_scanner)) / CLOCKS_PER_SEC;
    printf("\nScanning successful✅. Execution time: %f seconds\n\n", scanner_cpu_time_used);

    // **************************************** PARSING *********************************************

    // start parser timer
    puts("\n🕑 Starting Parsing\n..................................");
    clock_t start_parser, end_parser;
    double parser_cpu_time_used;
    start_parser = clock();

    //    entry point for the parser functions
    printf("\n%d.)   %s of type %d  in main. (start token) \n\n", pos, tokens[pos].value, tokens[pos].type);
    parser();
   if (tokens[pos].type == EOF)
    {
        // stop scanner timer
        end_parser = clock(); // -- Stop the timer
        parser_cpu_time_used = ((double)(end_parser - start_parser)) / CLOCKS_PER_SEC;
        printf("\nParsing successful✅. Execution time: %f seconds\n\n", parser_cpu_time_used);
    }
    else
    {
        printf("\n\n❌ Parser Failure.\n\n");

        end_parser = clock();
        exit(EXIT_FAILURE);
    }

    return 0;
}

// Parser functions
enum Token_Type next_token(char *location)
{
    pos++;

    // The location variable is just debugging purposes to show where exactly a token is being read from.
    printf("\n%d.)   %s of type %d  called in  %s\n\n", pos, tokens[pos].value, tokens[pos].type, location);

    return tokens[pos].type;
}

void parser()
{
    while (tokens[pos].type != EOF)
    {
        statement();
    }
}

void statement()
{
    switch (tokens[pos].type)
    {
    case KEYWORD:
        if (strcmp(tokens[pos].value, "int") == 0)
        {
            declaration();
        }
        if (strcmp(tokens[pos].value, "void") == 0)
        {
            declaration();
        }
        if (strcmp(tokens[pos].value, "char") == 0)
        {
            declaration();
        }
        else if (strcmp(tokens[pos].value, "if") == 0)
        {
            if_statement();
        }
        else if (strcmp(tokens[pos].value, "while") == 0)
        {
            while_statement();
        }
        else if (strcmp(tokens[pos].value, "return") == 0)
        {
            return_statement();
        }
        break;
    case IDENTIFIER:
        identifier();
        expression_statement();
        break;
    default:
        printf("\n\n❌ ERROR! Unrecognized syntax\n\n");
        exit(EXIT_FAILURE);
        break;
    }
}

void declaration()
{
    next_token("declaration"); // consume 'int' and go the next token
    identifier();
    if (strcmp(tokens[pos].value, ";") == 0)
    {
        variable_declaration();
    }
    else if (strcmp(tokens[pos].value, "(") == 0)
    {
        function_declaration();
    }
}
void variable_declaration()
{
    if (tokens[pos].type != SYMBOL || strcmp(tokens[pos].value, ";") == 1)
    {
        printf("\n\n❌ ERROR! Expected ';' at the end of variable declaration\n\n");
        exit(EXIT_FAILURE);
    }
    next_token("variable declaration 2"); // consume ';' and go the next token
}

void function_declaration()
{
    if (tokens[pos].type != SYMBOL || strcmp(tokens[pos].value, "(") == 1)
    {
        printf("\n\n❌ ERROR! Expected '(' after function name for function declaration\n\n");
        exit(EXIT_FAILURE);
    }
    next_token("function declaration 1"); // consume '(' and go the next token

    if (tokens[pos].type != SYMBOL || strcmp(tokens[pos].value, ")") == 1)
    {
        if (tokens[pos].type == KEYWORD && strcmp(tokens[pos].value, "void") == 0)
        {
            next_token("function declaration 2");
        }
        else if ((tokens[pos].type == KEYWORD && strcmp(tokens[pos].value, "int") == 0) || (tokens[pos].type == KEYWORD && strcmp(tokens[pos].value, "char") == 0) || (tokens[pos].type == KEYWORD && strcmp(tokens[pos].value, "float") == 0))
        {
            next_token("function declaration 3");
            identifier();
        }
    }
    if (tokens[pos].type != SYMBOL || strcmp(tokens[pos].value, ")") == 1)
    {
        printf("\n\n❌ ERROR! Expected ')' after function name for function declaration\n\n");
        exit(EXIT_FAILURE);
    }
    next_token("function declaration 3"); // consume ')' and go the next token
    block();
}

void expression_statement()
{
    if (tokens[pos].type == OPERATOR && strcmp(tokens[pos].value, "=") == 0)
    {
        next_token("expression_statement() 1");
        expression();
        if (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, ";") == 0)
        {
            next_token("expression_statement() 1"); // consume ';' and go the next token
        }
        else
        {
            printf("\n\n❌ ERROR! Expecting a ';'.\n\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, "(") == 0)
    {
        expression();

        if (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, ")") == 0)
        {
            next_token("expression statement 4"); // consume ')' and go the next token
        }
        else
        {
            printf("\n\n❌ ERROR! Closing parenthesis expected.\n\n");
            exit(EXIT_FAILURE);
        }
        if (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, ";") == 0)
        {
            next_token("expression_statement() 5"); // consume ';' and go the next token
        }
        else
        {
            printf("\n\n❌ ERROR! Expecting a ';'.\n\n");
            exit(EXIT_FAILURE);
        }
    }
}

void block()
{
    if (tokens[pos].type != SYMBOL || strcmp(tokens[pos].value, "{") == 1)
    {
        printf("\n\nSyntax error ❌: expected '{', found %s\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }
    next_token("block 1"); // consume '{' and go the next token
    while (tokens[pos].type != SYMBOL || strcmp(tokens[pos].value, "}") == 1)
    {
        statement();
    }

    if (tokens[pos].type && strcmp(tokens[pos].value, "}") == 0)
    {
        next_token("block 2");
    }
    else
    {
        printf("\n\n❌ ERROR! Closing '}' expected.\n\n");
        exit(EXIT_FAILURE);
    }
}

void if_statement()
{
    next_token("if statement 1"); // consume 'if' and go the next token
    if (tokens[pos].type != SYMBOL || strcmp(tokens[pos].value, "(") != 0)
    {
        printf("\n\nSyntax error ❌: expected '(', found %s\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }

    expression();

    if (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, ")") == 0)
    {
        next_token("if statement 2"); // consume ')' and go the next token
    }
    else
    {
        printf("\n\n❌ ERROR! Closing parenthesis expected.\n\n");
        exit(EXIT_FAILURE);
    }

    block();
    if (tokens[pos].type == KEYWORD && strcmp(tokens[pos].value, "else") == 0)
    {
        next_token("if statement 3"); // consume 'else' and go the next token
        block();
    }
}

void while_statement()
{

    next_token("while statement 1"); // consume 'while' and go the next token

    if (tokens[pos].type != SYMBOL || strcmp(tokens[pos].value, "(") != 0)
    {
        printf("\n\nSyntax error ❌: expected '(', found %s\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }

    expression();

    if (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, ")") == 0)
    {
        next_token("while statement 2"); // consume ')' and go the next token
    }
    else
    {
        printf("\n\n❌ ERROR! Closing parenthesis expected.\n\n");
        exit(EXIT_FAILURE);
    }

    block();
}

void return_statement()
{
    next_token("return_statement");          // consume 'return'
    if (strcmp(tokens[pos].value, "(") == 0) // return with brackets i.e return (0);
    {
        expression();
        if (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, ")") == 0)
        {
            next_token("expression statement 4"); // consume ')' and go the next token
        }
        else
        {
            printf("\n\n❌ ERROR! Closing parenthesis expected.\n\n");
            exit(EXIT_FAILURE);
        }
        if (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, ";") == 0)
        {
            next_token("expression_statement() 5"); // consume ';' and go the next token
        }
        else
        {
            printf("\n\n❌ ERROR! Expecting a ';'.\n\n");
            exit(EXIT_FAILURE);
        }
    }
    else // return without brackets i.e return 0;
    {
        expression();
        if (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, ";") == 0)
        {
            next_token("expression_statement() 5"); // consume ';' and go the next token
        }
        else
        {
            printf("\n\n❌ ERROR! Expecting a ';'.\n\n");
            exit(EXIT_FAILURE);
        }
    }
}

// ************************ how expressions are built by just calling expression() once ***************
// these parser functions build expressions from the inmost function - primary() - to the outermost -expression() in a recursive manner
// i.e going inwards upto the center, then building upwards while recursively going inwards again to comb through every function for matching tokens

// function to parse an expression
void expression()
{
    logical_or();
}

// logical OR operation
void logical_or()
{
    logical_and();
    while (tokens[pos].type == OPERATOR && strcmp(tokens[pos].value, "||") == 0)
    {
        next_token("logical_or");
        logical_and();
    }
}

// logical AND operation
void logical_and()
{
    equality();
    while (tokens[pos].type == OPERATOR && strcmp(tokens[pos].value, "&&") == 0)
    {
        next_token("logical_and");
        equality();
    }
}

// equality operator
void equality()
{
    comparison();
    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "==") == 0 || strcmp(tokens[pos].value, "!=") == 0))
    {
        next_token("equality");
        comparison();
    }
}

// comparison operator
void comparison()
{
    assignment();
    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, ">") == 0 || strcmp(tokens[pos].value, "<") == 0 ||
                                            strcmp(tokens[pos].value, ">=") == 0 || strcmp(tokens[pos].value, "<=") == 0))
    {
        next_token("comparison");
        assignment();
    }
}

// assignment operation
void assignment()
{
    term();
    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "=") == 0))
    {
        next_token("assignment"); // consume '=' and go to the next token
        term();
    }
}

// term operation
void term()
{
    factor();
    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "+") == 0 || strcmp(tokens[pos].value, "-") == 0))
    {
        next_token("term"); // consume '+' or '-' and go the next token
        factor();
    }
}

// factor operation
void factor()
{
    unary();
    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "*") == 0 || strcmp(tokens[pos].value, "/") == 0 || strcmp(tokens[pos].value, "%") == 0))
    {
        next_token("factor");
        unary();
    }
}

// unary operation
void unary()
{
    if (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "-") == 0 || strcmp(tokens[pos].value, "!") == 0))
    {
        next_token("unary");
    }
    primary();
}

// primary operation
void primary()
{
    if (tokens[pos].type == IDENTIFIER)
    {
        identifier();
    }
    else if (tokens[pos].type == NUMBER)
    {
        number();
    }
    else if (tokens[pos].type == STRING)
    {
        string();
    }
    else if (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, "(") == 0)
    {
        next_token("primary 1"); // consume '(' and go the next token
        expression();

        if (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, ",") == 0)
        {
            next_token("primary");

            if (strcmp(tokens[pos].value, "\"") == 0 || tokens[pos].type == STRING)
            {
                printf("\n\n❌ ERROR! String or '\"' not expected. Expected id, num or symbol, found %s\n", tokens[pos].value);
                exit(EXIT_FAILURE);
            }
            else if (tokens[pos].type == IDENTIFIER || tokens[pos].type == NUMBER)
            {
                expression();
            }

            else if (strcmp(tokens[pos].value, "&") == 0 && tokens[pos + 1].type == IDENTIFIER)
            {
                next_token("primary"); // consume '&' and go the next token
                expression();
            }
        }
    }
    else
    {
        printf("\n\n❌ ERROR! Invalid token: %s\n\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }
}
// *************************************************************************************

void identifier()
{
    if (tokens[pos].type == IDENTIFIER)
    {
        next_token("identifier"); // consume the identifier and go the next token
    }
    else
    {
        printf("\n\n❌ ERROR! Expected identifier, but got %s.\n\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }
}

void number()
{
    if (tokens[pos].type == NUMBER)
    {
        next_token("number"); // consume num and go the next token
    }
    else
    {
        printf("\n\n❌ ERROR! Expected number, but got %s.\n\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }
}

void string()
{
    if (tokens[pos].type == STRING)
    {
        next_token("string");
    }
    else
    {
        printf("\n\n❌ ERROR! Expected string, but got %s.\n\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }
}