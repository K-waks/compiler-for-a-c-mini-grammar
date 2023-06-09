#include "lexer.c"

int pos = 0; // indexes the tokens.
char *node_value;

/* ********************************** HELPER FUNCTIONS ******************************************/

Node *new_node(char *node_value, Node_Type type) // function to create a new node
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->type = type;
    node->num_children = 0;
    node->value = node_value;

    return node;
}

void add_child(Node *parent, Node *child) // function to add a child to a node in the parse tree
{
    parent->children[parent->num_children] = child;
    parent->num_children++;
}

void print_tree(Node *node, int depth, int index) // updated to include an integer variable index that tracks the index of the child
{
    int i;
    for (i = 0; i < depth; i++)
    {
        printf("    ");
    }

    if (node->type == TERMINAL)
        printf("\033[32m (%d.%d) %s \033[0m\n", depth, index, node->value);
    else
        printf("\033[34m (%d.%d) %s \033[0m\n", depth, index, node->value);

    for (i = 0; i < node->num_children; i++)
    {
        print_tree(node->children[i], depth + 1, i); // pass the index of the child to the recursive call
    }
}

/* *********************************** PARSER FUNCTIONS *****************************************/

// entry point for the parser functions
// ---------------------------------------------

void parser()
{
    node_value = "PROGRAM";
    root_node = new_node(node_value, PROGRAM);

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
        print_tree(root_node, 0, 0);
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
    node_value = "VARIABLE DECLARATION";
    Node *node = new_node(node_value, VARIABLE_DECLARATION);

    add_child(node, type_specifier()); // consume int, float, char or void and go to the next token
    add_child(node, identifier());     // consume identifier and go to the next token;
    add_child(node, match(";"));       // consume ';' and go to the next token

    return node;
}

Node *function_declaration()
{
    node_value = "FUNCTION DECLARATION";
    Node *node = new_node(node_value, FUNCTION_DECLARATION);

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
    node_value = "PARAMETER DECLARATION";
    Node *node = new_node(node_value, PARAMETER_DECLARATION);

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
    node_value = "BLOCK";
    Node *node = new_node(node_value, BLOCK);

    add_child(node, match("{")); // consume '{' and go to the next token

    while (strcmp(tokens[pos].value, "}") != 0)
    {
        if (tokens[pos].type == KEYWORD && (strcmp(tokens[pos].value, "int") == 0 || strcmp(tokens[pos].value, "float") == 0 || strcmp(tokens[pos].value, "char") == 0))
        {
            add_child(node, variable_declaration());
            
            while (tokens[pos].type == KEYWORD && (strcmp(tokens[pos].value, "int") == 0 || strcmp(tokens[pos].value, "float") == 0 || strcmp(tokens[pos].value, "char") == 0))
            {
                add_child(node, variable_declaration());
            }
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
    node_value = "IF STATEMENT";
    Node *node = new_node(node_value, IF_STATEMENT);

    add_child(node, match("if")); // consume 'if' and go to the next token
    add_child(node, match("("));  // consume '(' and go to the next token
    add_child(node, expression());
    add_child(node, match(")")); // consume ')' and go to the next token
    add_child(node, block());
    add_child(node, match("else")); // consume 'else' and go to the next token
    add_child(node, block());

    return node;
}

Node *while_statement()
{
    node_value = "WHILE STATEMENT";
    Node *node = new_node(node_value, WHILE_STATEMENT);

    add_child(node, match("while")); // consume 'while' and go to the next token
    add_child(node, match("("));     // consume '(' and go to the next token
    add_child(node, expression());
    add_child(node, match(")")); // consume ')' and go to the next token
    add_child(node, block());

    return node;
}

Node *expression_statement()
{
    Node *node = NULL;

    if (strcmp(tokens[pos].value, "printf") == 0) // e.g printf("Hello, World");
    {
        node = printf_statement();
    }
    else if (strcmp(tokens[pos].value, "scanf") == 0) // e.g scanf("%d", &a);
    {
        node = scanf_statement();
    }
    else if (tokens[pos].type == IDENTIFIER && strcmp(tokens[pos + 1].value, "(") == 0) // e.g sum();
    {
        node = function_call();
    }
    else if (tokens[pos].type == IDENTIFIER && strcmp(tokens[pos + 1].value, "=") == 0) // e.g a = 7;
    {
        node = assignment_statement();
    }

    add_child(node, match(";")); // consume ';' and go to the next token

    return node;
}

Node *printf_statement()
{
    node_value = "PRINTF STATEMENT";
    Node *node = new_node(node_value, PRINTF_STATEMENT);
    add_child(node, match("printf"));
    add_child(node, match("("));

    add_child(node, string());

    while (strcmp(tokens[pos].value, ",") == 0)
    {
        add_child(node, match(","));

        if (tokens[pos].type == IDENTIFIER && strcmp(tokens[pos + 1].value, "(") == 0)
        {
            add_child(node, function_call());
        }
        else
        {
            add_child(node, identifier());
        }
    }
    add_child(node, match(")"));

    return node;
}

Node *scanf_statement()
{
    node_value = "SCANF STATEMENT";
    Node *node = new_node(node_value, SCANF_STATEMENT);
    add_child(node, match("scanf"));
    add_child(node, match("("));

    add_child(node, string());

    while (strcmp(tokens[pos].value, ",") == 0)
    {
        add_child(node, match(","));
        add_child(node, match("&"));
        add_child(node, identifier());
    }
    add_child(node, match(")"));

    return node;
}

Node *assignment_statement()
{
    node_value = "ASSIGNMENT STATEMENT";
    Node *node = new_node(node_value, ASSIGNMENT_STATEMENT);

    Node *id_node = identifier();
    Node *op_node = match("=");
    add_child(op_node, id_node);
    add_child(op_node, expression());
    add_child(node, op_node);

    return node;
}

Node *function_call()
{
    node_value = "FUNCTION CALL";
    Node *node = new_node(node_value, FUNCTION_CALL);
    add_child(node, identifier());
    add_child(node, match("("));

    if (strcmp(tokens[pos].value, ")") != 0)
    {
        add_child(node, expression());
    }
    add_child(node, match(")"));

    return node;
}

Node *return_statement()
{
    node_value = "RETURN STATEMENT";
    Node *node = new_node(node_value, RETURN_STATEMENT);

    add_child(node, match("return")); // consume return and go to the next token
    add_child(node, match("("));      // consume '(' and go to the next token
    add_child(node, expression());
    add_child(node, match(")")); // consume ')' and go to the next token
    add_child(node, match(";")); // consume ';' and go to the next token

    return node;
}

// Expressions
// ---------------------------------------------

Node *expression()
{
    node_value = "EXPRESSION";
    Node *node = new_node(node_value, EXPRESSION);
    add_child(node, nested());
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
        node_value = "Unary";
        node = new_node(node_value, UNARY);
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
    {
        node = primary();
    }
    return node;
}

Node *primary()
{
    Node *node = NULL;

    if (tokens[pos].type == IDENTIFIER && strcmp(tokens[pos + 1].value, "(") == 0)
    {
        node = function_call();
    }

    else if (tokens[pos].type == IDENTIFIER)
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
        Node *nested = new_node("NESTED", NESTED);
        add_child(nested, match("(")); // consume '(' and go to the next token
        add_child(nested, expression());
        add_child(nested, match(")"));
        node = nested;
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
        printf("⛔ SYNTAX ERROR! Expected type-specifier, but got %s in %s.\n\n", tokens[pos].value, node_value);
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
        printf("⛔ SYNTAX ERROR! Expected identifier, but got %s in %s.\n\n", tokens[pos].value, node_value);
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
        printf("⛔ SYNTAX ERROR! Expected number, but got %s in %s.\n\n", tokens[pos].value, node_value);
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
        printf("⛔ SYNTAX ERROR! Expected string, but got %s in %s.\n\n", tokens[pos].value, node_value);
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
        printf("⛔ SYNTAX ERROR! Expected '%s' but got '%s' in %s\n\n", value, tokens[pos].value, node_value);
        exit(EXIT_FAILURE);
    }

    Node *node = new_node(tokens[pos].value, TERMINAL);
    pos++;

    return node;
}
