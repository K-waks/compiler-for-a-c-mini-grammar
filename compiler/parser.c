#include "lexer.c"

// indexes the tokens.
int pos = 0;

// function to create a new node
Node *new_node(char *value)
{
    Node *node = (Node *)malloc(sizeof(Node));
    strcpy(node->value, value);
    node->num_children = 0;
    return node;
}

// function to add a child to a node in the parse tree
void add_child(Node *parent, Node *child)
{
    parent->children[parent->num_children] = child;
    parent->num_children++;
}

// entry point for the parser functions
int parser()
{

    program();

    if (tokens[pos].type == EOF)
    {
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}

// Parser functions
// -------------------------------------------------------------
Node *program()
{
    Node *node = new_node("Program");

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
    Node *node = new_node("Variable Declaration");

    add_child(node, type_specifier()); // consume int, float, char or void and go to the next token
    add_child(node, identifier());     // consume identifier and go to the next token;
    add_child(node, match(";"));       // consume ';' and go to the next token

    return node;
}

Node *function_declaration()
{
    Node *node = new_node("Function Declaration");

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
    Node *node = new_node("Parameter Declaration");

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
    Node *node = new_node("Block");

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
    Node *node = new_node("if Statement");

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
    Node *node = new_node("while Statement");

    add_child(node, match("while")); // consume 'while' and go to the next token
    add_child(node, match("("));     // consume '(' and go to the next token
    add_child(node, expression());
    add_child(node, match(")")); // consume ')' and go to the next token
    add_child(node, block());

    return node;
}

Node *expression_statement()
{
    Node *node = new_node("Expression Statement");

    add_child(node, expression());
    add_child(node, match(";")); // consume ';' and go to the next token

    return node;
}

Node *return_statement()
{
    Node *node = new_node("return Statement");

    add_child(node, match("return")); // consume return and go to the next token
    add_child(node, match("("));      // consume '(' and go to the next token
    add_child(node, expression());
    add_child(node, match(")")); // consume ')' and go to the next token
    add_child(node, match(";")); // consume ';' and go to the next token

    return node;
}

// Expressions are built from the inmost function - primary() - to the outermost -expression() in a recursive manner
// i.e going inwards upto the center, then building upwards while recursively going inwards again to comb through every function for matching tokens

// function to parse an expression starts from here till primary()
Node *expression()
{
    nested();
}

Node *nested()
{
    logical_or();
    
    if (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, "(") == 0)
    {
        match("(");
        logical_or();

        while (strcmp(tokens[pos].value, ",") == 0)
        {
            match(",");
            expression();
        }
        
        match(")");
    }
}

// logical OR operation
Node *logical_or()
{
    printf("lOGICAL_OR->[");

    logical_and();
    while (tokens[pos].type == OPERATOR && strcmp(tokens[pos].value, "||") == 0)
    {
        match("||"); // consume '||' and go to the next token
        logical_and();
    }

    printf("]");
}

// logical AND operation
Node *logical_and()
{
    printf("lOGICAL_AND->[");

    equality();
    while (tokens[pos].type == OPERATOR && strcmp(tokens[pos].value, "&&") == 0)
    {
        match("&&"); // consume '&&' and go to the next token
        equality();
    }

    printf("]");
}

// equality operator
Node *equality()
{
    printf("EQUALITY->[");

    comparison();
    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "==") == 0 || strcmp(tokens[pos].value, "!=") == 0))
    {
        if (strcmp(tokens[pos].value, "==") == 0)
            match("=="); // consume '==' and go to the next token
        else if (strcmp(tokens[pos].value, "!=") == 0)
            match("!="); // consume '!=' and go to the next token

        comparison();
    }

    printf("]");
}

// comparison operator
Node *comparison()
{
    printf("COMPARISON->[");

    assignment();
    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, ">") == 0 || strcmp(tokens[pos].value, "<") == 0 ||
                                            strcmp(tokens[pos].value, ">=") == 0 || strcmp(tokens[pos].value, "<=") == 0))
    {
        if (strcmp(tokens[pos].value, ">") == 0)
            match(">"); // consume '>' and go to the next token
        else if (strcmp(tokens[pos].value, "<") == 0)
            match("<"); // consume '<' and go to the next token
        else if (strcmp(tokens[pos].value, ">=") == 0)
            match(">="); // consume '>=' and go to the next token
        else if (strcmp(tokens[pos].value, "<=") == 0)
            match("<="); // consume '<=' and go to the next token

        assignment();
    }

    printf("]");
}

// assignment operation
Node *assignment()
{
    printf("ASSIGNMENT->[");

    term();
    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "=") == 0))
    {
        match("="); // consume '=' and go to the next token
        term();
    }

    printf("]");
}

// term operation
Node *term()
{
    factor();
    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "+") == 0 || strcmp(tokens[pos].value, "-") == 0))
    {
        if (strcmp(tokens[pos].value, "+") == 0)
            match("+"); // consume '+' and go to the next token
        else if (strcmp(tokens[pos].value, "-") == 0)
            match("-"); // consume '-' and go to the next token

        factor();
    }
}

// factor operation
Node *factor()
{
    printf("FACTOR->[");

    unary();
    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "*") == 0 || strcmp(tokens[pos].value, "/") == 0 || strcmp(tokens[pos].value, "%") == 0))
    {
        if (strcmp(tokens[pos].value, "*") == 0)
            match("*"); // consume '*' and go to the next token
        else if (strcmp(tokens[pos].value, "/") == 0)
            match("/"); // consume '/' and go to the next token
        else if (strcmp(tokens[pos].value, "%") == 0)
            match("%"); // consume '%' and go to the next token

        unary();
    }

    printf("]");
}

// unary operation
Node *unary()
{
    if (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "-") == 0 || strcmp(tokens[pos].value, "!") == 0))
    {
        if (strcmp(tokens[pos].value, "-") == 0)
            match("-"); // consume '-' and go to the next token
        else if (strcmp(tokens[pos].value, "!") == 0)
            match("!"); // consume '!' and go to the next token
    }
    primary();
}

// primary operation
Node *primary()
{
    if (tokens[pos].type == IDENTIFIER)
    {
        identifier(); // consume identifier and go to the next token
    }
    else if (tokens[pos].type == NUMBER)
    {
        number(); // consume number and go to the next token
    }
    else if (tokens[pos].type == STRING)
    {
        string(); // consume string and go to the next token
    }
    else if (strcmp(tokens[pos].value, "(") == 0)
    {
        match("("); // consume '(' and go to the next token
        if (strcmp(tokens[pos].value, ")") != 0)
        {
            expression();
        }
        match(")");
    }
    else if (strcmp(tokens[pos].value, "&") == 0)
    {
        match("&"); // consume '(' and go to the next token
        identifier();
    }
    else
    {
        printf("\n\n❌ Syntax ERROR! Invalid token: %s\n\n", tokens[pos].value);
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }
}
// *************************************************************************************

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
