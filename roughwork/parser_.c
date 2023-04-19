#include <stdio.h>
#include <ctype.h>

#define MAX_TOKEN_LENGTH 100

// Token types
enum TokenType
{
    INT,
    ADD_OP,
    SUB_OP,
    MUL_OP,
    DIV_OP,
    LEFT_PAREN,
    RIGHT_PAREN,
    END_OF_INPUT,
    ERROR
};

// Token structure
struct Token
{
    enum TokenType type;
    char value[MAX_TOKEN_LENGTH];
};
// Global variables
struct Token current_token;
char current_char;
int pos = 0;
// Function declarations
void advance();
void skip_whitespace();
void skip_comment();
void get_token();
void factor();
void term();
void additive_expression();
void expression();
// Main function
int main()
{
    // Read input expression
    printf("Enter an expression: ");
    fgets(current_token.value, MAX_TOKEN_LENGTH, stdin);
    // Get first token
    advance();
    get_token();
    // Parse expression
    expression();
    // Check for end of input
    if (current_token.type != END_OF_INPUT)
    {
        printf("Error: unexpected end of input\n");
        return 1;
    }
    printf("Expression is valid\n");
    return 0;
}
// Helper function to read the next character of input
void advance() { current_char = current_token.value[pos++]; }
// Helper function to skip whitespace characters
void skip_whitespace()
{
    while (isspace(current_char))
    {
        advance();
    }
}
// Helper function to skip comment
void skip_comment()
{
    if (current_char == '#')
    {
        advance();
        while (current_char != '\n')
        {
            advance();
        }
        skip_whitespace();
        skip_comment();
    }
}
// Helper function to recognize and return the next token
void get_token()
{
    // Skip whitespace characters
    skip_whitespace();
    // Skip comments
    skip_comment();
    // Check for end of input
    if (current_char == '\0')
    {
        current_token.type = END_OF_INPUT;
        return;
    }
    // Check for integer token
    if (isdigit(current_char))
    {
        int i = 0;
        while (isdigit(current_char) && i < MAX_TOKEN_LENGTH)
        {
            current_token.value[i++] = current_char;
            advance();
        }
        current_token.value[i] = '\0';
        current_token.type = INT;
        return;
    }
    // Check for operator tokens
    switch (current_char)
    {
    case '+':
        current_token.type = ADD_OP;
        break;
    case '-':
        current_token.type = SUB_OP;
        break;
    case '*':
        current_token.type = MUL_OP;
        break;
    case '/':
        current_token.type = DIV_OP;
        break;
    case '(':
        current_token.type = LEFT_PAREN;
        break;
    case ')':
        current_token.type = RIGHT_PAREN;
        break;
    default:
        current_token.type = ERROR;
        break;
    }
    // Advance to next character
    advance();
}
// Factor production
void factor()
{
    if (current_token.type == INT)
    {
        // Integer token
        get_token();
    }
    else if (current_token.type == LEFT_PAREN)
    {
        // Left parenthesis
        get_token();
        expression();
        if (current_token.type != RIGHT_PAREN)
        {
            printf("Error: expected ')'\n");
            current_token.type = ERROR;
            return;
        }
        get_token();
    }
    else
    {
        // Invalid token
        printf("Error: invalid token\n");
        current_token.type = ERROR;
        return;
    }
}

// Term production
void term()
{
    // Factor
    factor();

    // Optional multiplication or division operator
    while (current_token.type == MUL_OP || current_token.type == DIV_OP)
    {
        // Get operator token
        enum TokenType op_type = current_token.type;
        get_token();

        // Factor
        factor();

        // Evaluate operation
        if (op_type == MUL_OP)
        {
            printf("Multiply\n");
        }
        else
        {
            printf("Divide\n");
        }
    }
}
// Additive expression production
void additive_expression()
{
    // Term
    term();

    // Optional addition or subtraction operator
    while (current_token.type == ADD_OP || current_token.type == SUB_OP)
    {
        // Get operator token
        enum TokenType op_type = current_token.type;
        get_token();

        // Term
        term();

        // Evaluate operation
        if (op_type == ADD_OP)
        {
            printf("Add\n");
        }
        else
        {
            printf("Subtract\n");
        }
    }
}

// Expression production
void expression()
{
    // Additive expression
    additive_expression();
}
