#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

enum Token
{
    IDENTIFIER,
    KEYWORD,
    OPERATOR,
    SYMBOL,
    NUMBER,
    STRING,
    COMMENT,
    NEWLINE
};

struct Node
{
    enum Token type;
    char *value;
    struct Node *left;
    struct Node *right;
};

enum Token current_token;
char current_value[256];
FILE *input;

void error(char *message)
{
    printf("Error: %s\n", message);
    exit(1);
}

void advance()
{
    int c;
    int buffer_len = 0;
    while ((c = fgetc(input)) != EOF)
    {
        if (isspace(c))
        {
            if (c == '\n')
            {
                current_token = NEWLINE;
            }
            continue;
        }
        else if (isalpha(c) || c == '_')
        {
            current_value[buffer_len++] = c;
            while ((c = fgetc(input)) != EOF && (isalnum(c) || c == '_'))
            {
                current_value[buffer_len++] = c;
            }
            ungetc(c, input);
            current_value[buffer_len] = '\0';
            if (strcmp(current_value, "int") == 0 || strcmp(current_value, "return") == 0)
            {
                current_token = KEYWORD;
            }
            else
            {
                current_token = IDENTIFIER;
            }
            return;
        }
        else if (isdigit(c))
        {
            current_value[buffer_len++] = c;
            while ((c = fgetc(input)) != EOF && (isdigit(c) || c == '.'))
            {
                current_value[buffer_len++] = c;
            }
            if (isalpha(c) || c == '_')
            {
                error("Invalid number");
            }
            ungetc(c, input);
            current_value[buffer_len] = '\0';
            current_token = NUMBER;
            return;
        }
        else if (c == '/' && (c = fgetc(input)) == '/')
        {
            while ((c = fgetc(input)) != EOF && c != '\n')
            {
            }
            current_token = COMMENT;
            return;
        }
        else if (c == '"')
        {
            current_value[buffer_len++] = c;
            while ((c = fgetc(input)) != EOF && c != '"')
            {
                if (c == '\\')
                {
                    current_value[buffer_len++] = c;
                    if ((c = fgetc(input)) != EOF)
                    {
                        current_value[buffer_len++] = c;
                    }
                    else
                    {
                        error("Unterminated string");
                    }
                }
                else
                {
                    current_value[buffer_len++] = c;
                }
            }
            if (c == EOF)
            {
                error("Unterminated string");
            }
            current_value[buffer_len++] = c;
            current_value[buffer_len] = '\0';
            current_token = STRING;
            return;
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '=' || c == '<' || c == '>' || c == '!' || c == '&' || c == '|')
        {
            current_value[buffer_len++] = c;
            if ((c = fgetc(input)) != EOF)
            {
                if ((current_value[0] == '<' && c == '<') || (current_value[0] == '>' && c == '>') || (current_value[0] == '&' && c == '&') || (current_value[0] == '|' && c == '|') || (current_value[0] == '=' && c == '=') || (current_value[0] == '!' && c == '='))
                {
                    current_value[buffer_len++] = c;
                }
                else
                {
                    ungetc(c, input);
                }
            }
            current_value[buffer_len] = '\0';
            current_token = OPERATOR;
            return;
        }
        else if (c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == ';' || c == ',' || c == ':')
        {
            current_value[0] = c;
            current_value[1] = '\0';
            current_token = SYMBOL;
            return;
        }
        else
        {
            error("Invalid character");
        }
    }
}

struct Node *parse_expression()
{
    struct Node *node = parse_logical_or_expression();

    while (current_token == OPERATOR && (strcmp(current_value, "==") == 0 || strcmp(current_value, "!=") == 0 || strcmp(current_value, "<") == 0 || strcmp(current_value, "<=") == 0 || strcmp(current_value, ">") == 0 || strcmp(current_value, ">=") == 0))
    {
        struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
        new_node->type = current_token;
        new_node->left = node;
        advance();
        new_node->right = parse_logical_or_expression();
        node = new_node;
    }

    return node;
}

struct Node *parse_logical_or_expression()
{
    struct Node *node = parse_logical_and_expression();

    while (current_token == OPERATOR && strcmp(current_value, "||") == 0)
    {
        struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
        new_node->type = current_token;
        new_node->left = node;
        advance();
        new_node->right = parse_logical_and_expression();
        node = new_node;
    }

    return node;
}

// struct Node *parse_logical_and_expression()
// {
//     struct Node *node =


struct Node *parse_primary_expression()
{
    struct Node *node = (struct Node *)malloc(sizeof(struct Node));
    node->left = NULL;
    node->right = NULL;

    if (current_token == IDENTIFIER || current_token == NUMBER || current_token == STRING)
    {
        node->type = current_token;
        node->value = strdup(current_value);
        advance();
    }
    else if (current_token == SYMBOL && current_value[0] == '(')
    {
        advance();
        node = parse_expression();
        if (current_token != SYMBOL || current_value[0] != ')')
        {
            error("Missing closing parenthesis");
        }
        advance();
    }
    else
    {
        error("Expected primary expression");
    }

    return node;
}

struct Node *parse_postfix_expression()
{
    struct Node *node = parse_primary_expression();

    while (current_token == SYMBOL && (current_value[0] == '(' || current_value[0] == '[' || current_value[0] == '.'))
    {
        struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
        new_node->left = node;
        new_node->right = NULL;

        if (current_value[0] == '(')
        {
            new_node->type = SYMBOL;
            new_node->value = strdup(current_value);
            advance();
            new_node->right = parse_expression();
            if (current_token != SYMBOL || current_value[0] != ')')
            {
                error("Missing closing parenthesis");
            }
            advance();
        }
        else if (current_value[0] == '[')
        {
            new_node->type = SYMBOL;
            new_node->value = strdup(current_value);
            advance();
            new_node->right = parse_expression();
            if (current_token != SYMBOL || current_value[0] != ']')
            {
                error("Missing closing square bracket");
            }
            advance();
        }
        else if (current_value[0] == '.')
        {
            advance();
            if (current_token != IDENTIFIER)
            {
                error("Expected identifier after '.'");
            }
            new_node->type = SYMBOL;
            new_node->value = strdup(".");
            strcat(new_node->value, current_value);
            advance();
        }

        node = new_node;
    }

    return node;
}

struct Node *parse_unary_expression()
{
    if (current_token == OPERATOR && (current_value[0] == '+' || current_value[0] == '-' || current_value[0] == '!' || current_value[0] == '~'))
    {
        struct Node *node = (struct Node *)malloc(sizeof(struct Node));
        node->type = OPERATOR;
        node->value = strdup(current_value);
        node->left = NULL;
        advance();
        node->right = parse_unary_expression();
        return node;
    }
    else
    {
        return parse_postfix_expression();
    }
}

struct Node *parse_multiplicative_expression()
{
    struct Node *node = parse_unary_expression();

    while (current_token == OPERATOR && (current_value[0] == '*' || current_value[0] == '/' || current_value[0] == '%'))
    {
        struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
        new_node->type = current_token;
        new_node->left = node;
        advance();
        new_node->right = parse_unary_expression();
        node = new_node;
    }

    return node;
}


struct Node *parse_additive_expression()
{
    struct Node *node = parse_multiplicative_expression();

    while (current_token == OPERATOR && (current_value[0] == '+' || current_value[0] == '-'))
    {
        struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
        new_node->type = OPERATOR;
        new_node->value = strdup(current_value);
        new_node->left = node;
        advance();
        new_node->right = parse_multiplicative_expression();
        node = new_node;
    }

    return node;
}

struct Node *parse_relational_expression()
{
    struct Node *node = parse_additive_expression();

    while (current_token == OPERATOR && (current_value[0] == '<' || current_value[0] == '>' || (current_value[0] == '=' && current_value[1] == '=')))
    {
        char op = current_value[0];
        advance();
        struct Node *right = parse_additive_expression();

        if (op == '<')
        {
            node = make_binary_operator_node(NODE_TYPE_LESS_THAN, node, right);
        }
        else if (op == '>')
        {
            node = make_binary_operator_node(NODE_TYPE_GREATER_THAN, node, right);
        }
        else
        {
            node = make_binary_operator_node(NODE_TYPE_EQUAL_TO, node, right);
        }
    }

    return node;
}

struct Node *parse_postfix_expression()
{
    struct Node *node = parse_primary_expression();

    while (current_token == SYMBOL && (current_value[0] == '(' || current_value[0] == '[' || current_value[0] == '.'))
    {
        struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
        new_node->left = node;
        new_node->right = NULL;

        if (current_value[0] == '(')
        {
            advance();
            new_node->type = SYMBOL;
            new_node->value = strdup("(");
            new_node->right = parse_expression();
            if (current_token != SYMBOL || current_value[0] != ')')
            {
                error("Missing closing parenthesis");
            }
            advance();
        }
        else if (current_value[0] == '[')
        {
            advance();
            new_node->type = SYMBOL;
            new_node->value = strdup("[");
            new_node->right = parse_expression();
            if (current_token != SYMBOL || current_value[0] != ']')
            {
                error("Missing closing bracket");
            }
            advance();
        }
        else if (current_value[0] == '.')
        {
            advance();
            if (current_token != IDENTIFIER)
            {
                error("Expected identifier after '.'");
            }
            new_node->type = SYMBOL;
            new_node->value = strdup(".");
            new_node->right = (struct Node *)malloc(sizeof(struct Node));
            new_node->right->type = IDENTIFIER;
            new_node->right->value = strdup(current_value);
            new_node->right->left = NULL;
            new_node->right->right = NULL;
            advance();
        }
        node = new_node;
    }

    return node;
}
