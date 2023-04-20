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
enum Token_Type next_token();
void program();
void statement();
void variable_declaration();
void function_declaration();
void identifier();
void block();

// file descriptor variable declared and buffer of dynamic size to hold the read characters
FILE *fp;
char buffer[BUFSIZ];

// tokens will be in the form of structs so that their attributes can be filled and accessed.
struct Token
{
    enum Token_Type type;
    char value[256];
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
    puts("\nScanning successful✅ !");

    // **************************************** PARSING *********************************************
    printf("\n%d   %s   %d\n\n", tokens[pos].type, tokens[pos].value, pos);
    program();
    if (tokens[pos].type == EOF)
    {
        puts("\nParsing successful✅ !");
    }
    else
    {
        printf("\n\n❌ Parser Failure.\n\n");
        exit(EXIT_FAILURE);
    }

    // ************************************* CODE GENERATION *****************************************

    return 0;
}

// Parser functions
enum Token_Type next_token()
{
    pos++;
    printf("\n%d   %s   %d\n\n", tokens[pos].type, tokens[pos].value, pos);
    return tokens[pos].type;
}

void program()
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
            variable_declaration();
        }
        else if (strcmp(tokens[pos].value, "void") == 0)
        {
            function_declaration();
        }

        break;
    case IDENTIFIER:
        break;
    }
}

void variable_declaration()
{
    next_token();
    identifier();
    if (tokens[pos].type != SYMBOL || strcmp(tokens[pos].value, ";") == 1)
    {
        printf("\n\n❌ ERROR! Expected ';' at the end of variable declaration\n\n");
        exit(EXIT_FAILURE);
    }
    next_token(); // consume ';' and go the next token
}

void identifier()
{
    if (tokens[pos].type == IDENTIFIER)
    {
        next_token();
    }
    else
    {
        printf("\n\n❌ ERROR! Expected identifier, but got %s.\n\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }
}

void function_declaration()
{
    next_token(); // consume 'void' and go the next token
    identifier();
    if (tokens[pos].type != SYMBOL || strcmp(tokens[pos].value, "(") == 1)
    {
        printf("\n\n❌ ERROR! Expected '(' after function name for function declaration\n\n");
        exit(EXIT_FAILURE);
    }
    next_token(); // consume '(' and go the next token
    if (tokens[pos].type != SYMBOL || strcmp(tokens[pos].value, ")") == 1)
    {
        printf("\n\n❌ ERROR! Expected ')' after function name for function declaration\n\n");
        exit(EXIT_FAILURE);
    }
    next_token(); // consume ')' and go the next token
    block();
}

void block()
{
    if (tokens[pos].type != SYMBOL || strcmp(tokens[pos].value, "{") == 1)
    {
        printf("\n\nSyntax error ❌: expected '{', found %s\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }
    while (!(tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, "}") == 0))
    {
        next_token();
        statement();
    }
    // next_token();
}
