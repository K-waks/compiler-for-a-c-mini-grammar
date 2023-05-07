#include "lexer.c"

int pos = 0; // indexes the tokens.

int parser() // entry point for the parser functions
{

    Node *root = program();

    if (tokens[pos].type == EOF)
    {
        print_tree(root, 0);

        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}

/* **********************************************PARSER FUNCTIONS ******************************************************/

Node *program()
{
    Node *node = new_node("PROGRAM");

    while (tokens[pos].type != EOF)
    {
        add_child(node, declaration());
    }

    return node;
}

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
        printf("\n\n❌ Syntax ERROR! Expected declaration. (Note: Statements should not be written globally but inside funtions.)\n\n");
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }

    return node;
}

Node *variable_declaration()
{
    Node *node = new_node("VARIABLE DECLARATION");

    add_child(node, type_specifier()); // consume int, float, char or void and go to the next token
    add_child(node, identifier());     // consume identifier and go to the next token;
    add_child(node, match(";"));       // consume ';' and go to the next token

    return node;
}

Node *function_declaration()
{
    Node *node = new_node("FUNCTION DECLARATION");

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
    Node *node = new_node("PARAMETER DECLARATION");

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
    Node *node = new_node("BLOCK");

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
    Node *node = new_node("IF STATEMENT");

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
    Node *node = new_node("WHILE STATEMENT");

    add_child(node, match("while")); // consume 'while' and go to the next token
    add_child(node, match("("));     // consume '(' and go to the next token
    add_child(node, expression());
    add_child(node, match(")")); // consume ')' and go to the next token
    add_child(node, block());

    return node;
}

Node *expression_statement()
{
    Node *node = new_node("EXPRESSION STATEMENT");

    add_child(node, expression());
    add_child(node, match(";")); // consume ';' and go to the next token

    return node;
}

Node *return_statement()
{
    Node *node = new_node("RETURN STATEMENT");

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
        add_child(node, expression());

    add_child(node, match(";")); // consume ';' and go to the next token

    return node;
}

// Building Expressions recursively
// ---------------------------------------------

Node *expression()
{
    Node *node = nested();
    return node;
}

Node *nested()
{
    Node *node = logical_or();

    if (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, "(") == 0)
    {
        add_child(node, match("("));
        add_child(node, logical_or());

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

// logical OR operation
Node *logical_or()
{
    Node *node = logical_and();
    while (tokens[pos].type == OPERATOR && strcmp(tokens[pos].value, "||") == 0)
    {
        add_child(node, match("||")); // consume '||' and go to the next token
        add_child(node, logical_and());
    }

    return node;
}

// logical AND operation
Node *logical_and()
{
    Node *node = equality();
    while (tokens[pos].type == OPERATOR && strcmp(tokens[pos].value, "&&") == 0)
    {
        add_child(node, match("&&")); // consume '&&' and go to the next token
        add_child(node, equality());
    }

    return node;
}

// equality operator
Node *equality()
{
    Node *node = comparison();
    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "==") == 0 || strcmp(tokens[pos].value, "!=") == 0))
    {
        if (strcmp(tokens[pos].value, "==") == 0)
            add_child(node, match("==")); // consume '==' and go to the next token
        else if (strcmp(tokens[pos].value, "!=") == 0)
            add_child(node, match("!=")); // consume '!=' and go to the next token

        add_child(node, comparison());
    }

    return node;
}

// comparison operator
Node *comparison()
{
    Node *node = assignment();
    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, ">") == 0 || strcmp(tokens[pos].value, "<") == 0 ||
                                            strcmp(tokens[pos].value, ">=") == 0 || strcmp(tokens[pos].value, "<=") == 0))
    {
        if (strcmp(tokens[pos].value, ">") == 0)
            add_child(node, match(">")); // consume '>' and go to the next token
        else if (strcmp(tokens[pos].value, "<") == 0)
            add_child(node, match("<")); // consume '<' and go to the next token
        else if (strcmp(tokens[pos].value, ">=") == 0)
            add_child(node, match(">=")); // consume '>=' and go to the next token
        else if (strcmp(tokens[pos].value, "<=") == 0)
            add_child(node, match("<=")); // consume '<=' and go to the next token

        add_child(node, assignment());
    }

    return node;
}

// assignment operation
Node *assignment()
{
    Node *node = term();
    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "=") == 0))
    {
        add_child(node, match("=")); // consume '=' and go to the next token
        add_child(node, term());
    }

    return node;
}

// term operation
Node *term()
{
    Node *node = factor();
    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "+") == 0 || strcmp(tokens[pos].value, "-") == 0))
    {
        if (strcmp(tokens[pos].value, "+") == 0)
            add_child(node, match("+")); // consume '+' and go to the next token
        else if (strcmp(tokens[pos].value, "-") == 0)
            add_child(node, match("-")); // consume '-' and go to the next token

        add_child(node, factor());
    }

    return node;
}

// factor operation
Node *factor()
{
    Node *node = unary();
    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "*") == 0 || strcmp(tokens[pos].value, "/") == 0 || strcmp(tokens[pos].value, "%") == 0))
    {
        if (strcmp(tokens[pos].value, "*") == 0)
            add_child(node, match("*")); // consume '*' and go to the next token
        else if (strcmp(tokens[pos].value, "/") == 0)
            add_child(node, match("/")); // consume '/' and go to the next token
        else if (strcmp(tokens[pos].value, "%") == 0)
            add_child(node, match("%")); // consume '%' and go to the next token

        Node *node = unary();
    }

    return node;
}

// unary operation
Node *unary()
{
    Node *node = NULL;
    if (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "+") == 0 || strcmp(tokens[pos].value, "-") == 0 || strcmp(tokens[pos].value, "!") == 0))
    {
        node = new_node("UNARY EXPRESSION");
        add_child(node, match(tokens[pos].value)); // consume either '+', '-' or '!' and go to the next token
        add_child(node, primary());
    }
    else
        node = primary();

    return node;
}

// primary operation
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
        printf("\n\n❌ Syntax ERROR! Invalid token: %s\n\n", tokens[pos].value);
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }
    return node;
}

// ---------------------------------------------

Node *type_specifier()
{
    if (!(strcmp(tokens[pos].value, "int") == 0 || strcmp(tokens[pos].value, "void") == 0 || strcmp(tokens[pos].value, "char") == 0 || strcmp(tokens[pos].value, "float") == 0))
    {
        printf("\n\n❌ Syntax ERROR! Expected type-specifier\n\n");
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }
    Node *node = new_node(tokens[pos].value);
    pos++;

    return node;
}

Node *identifier()
{
    if (tokens[pos].type != IDENTIFIER)
    {
        printf("\n\n❌ Syntax ERROR! Expected identifier, but got %s.\n\n", tokens[pos].value);
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }
   Node *node = new_node(tokens[pos].value);
    pos++;

    return node;
}

Node *number()
{
    if (tokens[pos].type != NUMBER)
    {
        printf("\n\n❌ Syntax ERROR! Expected number, but got %s.\n\n", tokens[pos].value);
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }
    Node *node = new_node(tokens[pos].value);
    pos++;

    return node;
}

Node *string()
{
    if (tokens[pos].type != STRING)
    {
        printf("\n\n❌ Syntax ERROR! Expected string, but got %s.\n\n", tokens[pos].value);
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }
    Node *node = new_node(tokens[pos].value);
    pos++;

    return node;
}

Node *match(char *value)
{
    Node *node = NULL;

    if (strcmp(tokens[pos].value, value) != 0)
    {
        printf("\n\n❌ Syntax ERROR! Expected '%s' but got '%s'\n\n", value, tokens[pos].value);
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        node = new_node(tokens[pos].value);
        pos++;
    }

    return node;
}

/* ************************************************** HELPER FUNCTIONS ******************************************/

Node *new_node(char *value) // function to create a new node
{
    Node *node = (Node *)malloc(sizeof(Node));
    strcpy(node->value, value);
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
        printf("  ");
    }

    printf("%s", node->value);

    for (i = 0; i < node->num_children; i++)
    {
        printf("\n");
        print_tree(node->children[i], depth + 1);
    }
}
