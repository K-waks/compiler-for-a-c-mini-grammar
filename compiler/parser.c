#include "lexer.c"

int pos = 0; // indexes the tokens.

/* ********************************** HELPER FUNCTIONS ******************************************/

Node *new_node(char *value, Node_Type type) // function to create a new node
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->value = value;
    node->type = type;
    node->num_children = 0;
    return node;
}

void add_child(Node *parent, Node *child) // function to add a child to a node in the parse tree
{
    parent->children[parent->num_children] = child;
    parent->num_children++;
}

void print_tree(Node *node, int depth) // function to display parse tree onto the terminal
{
    int i;

    for (i = 0; i < depth; i++)
    {
        printf("    ");
    }

    if (node->type == NON_TERMINAL)
    {
        printf("\033[32m%s\033[0m\n", node->value); // non-terminals are displayed in color green
    }
    else
    {
        printf("\033[34m%s\033[0m\n", node->value); // terminals in color blue
    }

    for (i = 0; i < node->num_children; i++)
    {
        printf("\n");
        print_tree(node->children[i], depth + 1);
    }
}

/* *********************************** PARSER FUNCTIONS *****************************************/

// entry point for the parser functions
// ---------------------------------------------

void parser()
{
    root_node = new_node("PROGRAM", NON_TERMINAL);

    while (tokens[pos].type != EOF)
    {
        add_child(root_node, declaration());
    }

    if (tokens[pos].type != EOF)
    {
        puts("⛔ PARSER MALFUNCTION! Failed to scan all tokens.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        print_tree(root_node, 0);
    }
}

// Declarations
// ---------------------------------------------

Node *declaration()
{
    Node *node = NULL;

    if (tokens[pos].type == KEYWORD && tokens[pos + 1].type == IDENTIFIER && strcmp(tokens[pos + 2].value, "(") == 0)
    {
        node = function_declaration();
    }
    else if (tokens[pos].type == KEYWORD && tokens[pos + 1].type == IDENTIFIER)
    {
        node = variable_declaration();
    }
    else
    {
        puts("⛔ SYNTAX ERROR! Expected declaration. (Note: Statements should not be written globally but inside funtions.)\n");
        exit(EXIT_FAILURE);
    }

    return node;
}

Node *variable_declaration()
{
    Node *node = new_node("VARIABLE DECLARATION", NON_TERMINAL);

    add_child(node, type_specifier()); // consume int, float, char or void and go to the next token
    add_child(node, identifier());     // consume identifier and go to the next token;
    add_child(node, match(";"));       // consume ';' and go to the next token

    return node;
}

Node *function_declaration()
{
    Node *node = new_node("FUNCTION DECLARATION", NON_TERMINAL);

    add_child(node, type_specifier()); // consume int, float, char or void and go to the next token
    add_child(node, identifier());     // consume identifier and go to the next token
    add_child(node, match("("));       // consume '(' and go to the next token

    if (strcmp(tokens[pos].value, ")") != 0)
    {
        add_child(node, parameter_declaration());
    }

    add_child(node, match(")")); // consume ')' and go to the next token
    add_child(node, block());

    return node;
}

Node *parameter_declaration()
{
    Node *node = new_node("PARAMETER DECLARATION", NON_TERMINAL);

    add_child(node, type_specifier()); // consume int, char or float and go to the next token
    add_child(node, identifier());     // consume identifier and go to the next token

    while (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, ",") == 0)
    {
        add_child(node, match(","));       // consume ',' and go to the next token
        add_child(node, type_specifier()); // consume int, char or float and go to the next token
        add_child(node, identifier());     // consume identifier and go to the next token
    }

    return node;
}

Node *block()
{
    Node *node = new_node("BLOCK", NON_TERMINAL);

    add_child(node, match("{")); // consume '{' and go to the next token

    while (strcmp(tokens[pos].value, "}") != 0)
    {
        if (tokens[pos].type == KEYWORD && (strcmp(tokens[pos].value, "int") == 0 || strcmp(tokens[pos].value, "float") == 0 || strcmp(tokens[pos].value, "char") == 0))
        {
            add_child(node, variable_declaration());
        }

        add_child(node, statement());
    }

    add_child(node, match("}")); // consume '}' and go to the next token

    return node;
}

// Statements
// ---------------------------------------------

Node *statement()
{
    Node *node = NULL;

    if (strcmp(tokens[pos].value, "if") == 0)
    {
        node = if_statement();
    }
    else if (strcmp(tokens[pos].value, "while") == 0)
    {
        node = while_statement();
    }
    else if (strcmp(tokens[pos].value, "return") == 0)
    {
        node = return_statement();
    }
    else
    {
        node = expression_statement();
    }

    return node;
}

Node *if_statement()
{
    Node *node = new_node("IF STATEMENT", NON_TERMINAL);

    add_child(node, match("if")); // consume 'if' and go to the next token
    add_child(node, match("("));  // consume '(' and go to the next token
    add_child(node, expression());
    add_child(node, match(")")); // consume ')' and go to the next token
    add_child(node, block());

    if (tokens[pos].type == KEYWORD && strcmp(tokens[pos].value, "else") == 0)
    {
        add_child(node, match("else")); // consume 'else' and go to the next token
        add_child(node, block());
    }

    return node;
}

Node *while_statement()
{
    Node *node = new_node("WHILE STATEMENT", NON_TERMINAL);

    add_child(node, match("while")); // consume 'while' and go to the next token
    add_child(node, match("("));     // consume '(' and go to the next token
    add_child(node, expression());
    add_child(node, match(")")); // consume ')' and go to the next token
    add_child(node, block());

    return node;
}

Node *expression_statement()
{
    Node *node = new_node("EXPRESSION STATEMENT", NON_TERMINAL);

    add_child(node, expression());
    add_child(node, match(";")); // consume ';' and go to the next token

    return node;
}

Node *return_statement()
{
    Node *node = new_node("RETURN STATEMENT", NON_TERMINAL);

    add_child(node, match("return")); // consume return and go to the next token

    // return with brackets e.g. return (0);
    if (strcmp(tokens[pos].value, "(") == 0)
    {
        add_child(node, match("(")); // consume '(' and go to the next token
        add_child(node, expression());
        add_child(node, match(")")); // consume ')' and go to the next token
    }
    // return withiout brackets e.g. return 0;
    else
    {
        add_child(node, expression());
    }

    add_child(node, match(";")); // consume ';' and go to the next token

    return node;
}

// Expressions
// ---------------------------------------------

Node *expression()
{
    Node *node = nested();
    return node;
}

Node *nested()
{
    Node *node = assignment();

    if (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, "(") == 0)
    {
        add_child(node, match("("));
        add_child(node, assignment());

        while (strcmp(tokens[pos].value, ",") == 0)
        {
            add_child(node, match(","));
            add_child(node, expression());

            if (strcmp(tokens[pos - 1].value, "&") == 0)
            {
                add_child(node, identifier());
            }
        }

        add_child(node, match(")"));
    }

    return node;
}

Node *assignment()
{
    Node *node = logical_or();

    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "=") == 0))
    {
        Node *op_node = match("=");
        add_child(op_node, node);
        add_child(op_node, logical_or());
        node = op_node;
    }

    return node;
}

Node *logical_or()
{
    Node *node = logical_and();

    while (tokens[pos].type == OPERATOR && strcmp(tokens[pos].value, "||") == 0)
    {
        Node *op_node = match("||");
        add_child(op_node, node);
        add_child(op_node, logical_and());
        node = op_node;
    }

    return node;
}

Node *logical_and()
{
    Node *node = equality();

    while (tokens[pos].type == OPERATOR && strcmp(tokens[pos].value, "&&") == 0)
    {
        Node *op_node = match("&&");
        add_child(op_node, node);
        add_child(op_node, equality());
        node = op_node;
    }

    return node;
}

Node *equality()
{
    Node *node = comparison();

    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "==") == 0 || strcmp(tokens[pos].value, "!=") == 0))
    {
        Node *op_node;

        if (strcmp(tokens[pos].value, "==") == 0)
        {
            op_node = match("==");
            add_child(op_node, node);
        }
        else if (strcmp(tokens[pos].value, "!=") == 0)
        {
            op_node = match("!=");
            add_child(op_node, node);
        }

        add_child(op_node, comparison());
        node = op_node;
    }

    return node;
}

Node *comparison()
{
    Node *node = term();

    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, ">") == 0 || strcmp(tokens[pos].value, "<") == 0 ||
                                            strcmp(tokens[pos].value, ">=") == 0 || strcmp(tokens[pos].value, "<=") == 0))
    {
        Node *op_node;
        if (strcmp(tokens[pos].value, ">") == 0)
        {
            op_node = match(">");
            add_child(op_node, node);
        }
        else if (strcmp(tokens[pos].value, "<") == 0)
        {
            op_node = match("<");
            add_child(op_node, node);
        }
        else if (strcmp(tokens[pos].value, ">=") == 0)
        {
            op_node = match(">=");
            add_child(op_node, node);
        }
        else if (strcmp(tokens[pos].value, "<=") == 0)
        {
            op_node = match("<=");
            add_child(op_node, node);
        }

        add_child(op_node, term());
        node = op_node;
    }

    return node;
}

Node *term()
{
    Node *node = factor();

    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "+") == 0 || strcmp(tokens[pos].value, "-") == 0))
    {
        Node *op_node;
        if (strcmp(tokens[pos].value, "+") == 0)
        {
            op_node = match("+");
            add_child(op_node, node);
        }
        else if (strcmp(tokens[pos].value, "-") == 0)
        {
            op_node = match("-");
            add_child(op_node, node);
        }
        add_child(op_node, factor());
        node = op_node;
    }

    return node;
}

Node *factor()
{
    Node *node = unary();

    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "*") == 0 || strcmp(tokens[pos].value, "/") == 0 || strcmp(tokens[pos].value, "%") == 0))
    {
        Node *op_node;
        if (strcmp(tokens[pos].value, "*") == 0)
        {
            op_node = match("*");
            add_child(op_node, node);
        }
        else if (strcmp(tokens[pos].value, "/") == 0)
        {
            op_node = match("/");
            add_child(op_node, node);
        }
        else if (strcmp(tokens[pos].value, "%") == 0)
        {
            op_node = match("%");
            add_child(op_node, node);
        }

        add_child(op_node, unary());
        node = op_node;
    }

    return node;
}

Node *unary()
{
    Node *node = NULL;

    if (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "-") == 0 || strcmp(tokens[pos].value, "!") == 0))
    {
        node = new_node("UNARY EXPRESSION", NON_TERMINAL);
        if (strcmp(tokens[pos].value, "-") == 0)
        {
            add_child(node, match("-")); // consume '-' and go to the next token
        }

        else if (strcmp(tokens[pos].value, "!") == 0)
        {
            add_child(node, match("!")); // consume '!' and go to the next token
        }
        add_child(node, primary());
    }
    else
        node = primary();

    return node;
}

Node *primary()
{
    Node *node = NULL;

    if (tokens[pos].type == IDENTIFIER)
    {
        node = identifier(); // consume identifier and go to the next token
    }
    else if (tokens[pos].type == NUMBER)
    {
        node = number(); // consume number and go to the next token
    }
    else if (tokens[pos].type == STRING)
    {
        node = string(); // consume string and go to the next token
    }
    else if (strcmp(tokens[pos].value, "(") == 0)
    {
        add_child(node, match("(")); // consume '(' and go to the next token
        add_child(node, expression());
        add_child(node, match(")"));
    }
    else if (strcmp(tokens[pos].value, "&") == 0)
    {
        node = match("&"); // consume '&' and go to the next token
    }
    else
    {
        printf("⛔ SYNTAX ERROR! Unexpected token: %s\n\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }

    return node;
}

// Match functions
// ---------------------------------------------

Node *type_specifier()
{
    if (!(strcmp(tokens[pos].value, "int") == 0 || strcmp(tokens[pos].value, "void") == 0 || strcmp(tokens[pos].value, "char") == 0 || strcmp(tokens[pos].value, "float") == 0))
    {
        printf("⛔ SYNTAX ERROR! Expected type-specifier, but got %s.\n\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }

    Node *node = new_node(tokens[pos].value, TERMINAL);
    pos++;

    return node;
}

Node *identifier()
{
    if (tokens[pos].type != IDENTIFIER)
    {
        printf("⛔ SYNTAX ERROR! Expected identifier, but got %s.\n\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }

    Node *node = new_node(tokens[pos].value, TERMINAL);
    pos++;

    return node;
}

Node *number()
{
    if (tokens[pos].type != NUMBER)
    {
        printf("⛔ SYNTAX ERROR! Expected number, but got %s.\n\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }

    Node *node = new_node(tokens[pos].value, TERMINAL);
    pos++;

    return node;
}

Node *string()
{
    if (tokens[pos].type != STRING)
    {
        printf("⛔ SYNTAX ERROR! Expected string, but got %s.\n\n", tokens[pos].value);
        exit(EXIT_FAILURE);
    }

    Node *node = new_node(tokens[pos].value, TERMINAL);
    pos++;

    return node;
}

Node *match(char *value)
{
    if (strcmp(tokens[pos].value, value) != 0)
    {
        printf("⛔ SYNTAX ERROR! Expected '%s' but got '%s'\n\n", value, tokens[pos].value);
        exit(EXIT_FAILURE);
    }

    Node *node = new_node(tokens[pos].value, TERMINAL);
    pos++;

    return node;
}
