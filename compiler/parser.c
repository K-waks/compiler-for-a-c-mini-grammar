// indexes the tokens.
int pos = 0;

// entry point for the parser functions
int parser()
{

    program();

    if (tokens[pos].type == EOF)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}

// Parser functions
// -------------------------------------------------------------
void program()
{
    printf("PROGRAM->[");

    while (tokens[pos].type != EOF)
    {
        declaration();
    }

    printf("]");
}

void declaration()
{
    printf("DECLARATION->[");

    if (tokens[pos].type == KEYWORD && tokens[pos + 1].type == IDENTIFIER && strcmp(tokens[pos + 2].value, "(") == 0)
    {
        type_specifier(); // consume int, float, char or void and go to the next token
        function_declaration();
    }
    else if (tokens[pos].type == KEYWORD && tokens[pos + 1].type == IDENTIFIER)
    {
        type_specifier(); // consume int, float, char or void and go to the next token
        variable_declaration();
    }
    else
    {
        printf("\n\n❌ Syntax ERROR! Expected declaration. (Note: Statements should not be written globally but inside funtions.)\n\n");
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }

    printf("]");
}

void variable_declaration()
{
    printf("VARIABLE DECLARATION->[");

    if (tokens[pos].type == IDENTIFIER && strcmp(tokens[pos + 1].value, "=") == 0)
    {
        assignment(); // declaration with assignment operation e.g int hello = 3;
    }
    else
    {
        identifier(); // consume identifier and go to the next token   |    declaration only e.g int hello;
    }

    while (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, ",") == 0) // comma separated declarations e.g int hello, input, count = 0;
    {
        match(","); // consume ',' and go to the next token
        if (tokens[pos].type == IDENTIFIER && strcmp(tokens[pos + 1].value, "=") == 0)
        {
            assignment();
        }
        else
        {
            identifier(); // consume identifier and go to the next token
        }
    }

    if (!strcmp(tokens[pos].value, ";") == 0) // declarations must end with semi-colon
    {
        printf("\n\n❌ Syntax ERROR! Expected ';' at the end of variable declaration\n\n");
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }
    match(";"); // consume ';' and go to the next token

    printf("]");
}

void function_declaration()
{
    printf("FUNCTION DECLARATION->[");

    identifier(); // consume identifier and go to the next token
    match("(");   // consume '(' and go to the next token

    if (!strcmp(tokens[pos].value, ")") == 0)
        parameter_declaration();

    if (!strcmp(tokens[pos].value, ")") == 0)
    {
        printf("\n\n❌ Syntax ERROR! Expected ')' after function name for function declaration\n\n");
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }
    match(")"); // consume ')' and go to the next token
    block();

    printf("]");
}

void parameter_declaration()
{
    printf("PARAMETER DECLARATION->[");

    if (tokens[pos].type == KEYWORD && strcmp(tokens[pos].value, "void") == 0)
    {
        type_specifier(); // consume void only and go to the next token
    }
    else
    {
        type_specifier(); // consume int, char or float and go to the next token
        identifier();     // consume identifier and go to the next token
        while (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, ",") == 0)
        {
            match(","); // consume ',' and go to the next token
            if (tokens[pos].type == KEYWORD && strcmp(tokens[pos].value, "void") == 0)
            {
                printf("\n\n❌ Syntax ERROR! 'void' keyword argument not expected after ','.\n\n");
                printf("\n🚫 Parser FAILURE!\n\n");
                exit(EXIT_FAILURE);
            }

            type_specifier(); // consume int, char or float and go to the next token
            identifier();     // consume identifier and go to the next token
        }
    }

    printf("]");
}

void block()
{
    printf("BLOCK->[");

    if (!strcmp(tokens[pos].value, "{") == 0)
    {
        printf("\n\n❌ Syntax ERROR! Expected '{', found %s\n", tokens[pos].value);
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }
    match("{"); // consume '{' and go to the next token
    while (!strcmp(tokens[pos].value, "}") == 0)
    {
        if (tokens[pos].type == KEYWORD && (strcmp(tokens[pos].value, "int") == 0 || strcmp(tokens[pos].value, "float") == 0 || strcmp(tokens[pos].value, "char") == 0))
        {
            type_specifier(); // consume int, char or float and go to the next token
            variable_declaration();
        }
        statement();
    }
    if (!strcmp(tokens[pos].value, "}") == 0)
    {
        printf("\n\n❌ Syntax ERROR! Closing '}' expected.\n\n");
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }
    match("}"); // consume '}' and go to the next token

    printf("]");
}

void statement()
{
    printf("STATEMENT->[");

    if (tokens[pos].type == KEYWORD && strcmp(tokens[pos].value, "if") == 0)
    {
        if_statement();
    }
    else if (tokens[pos].type == KEYWORD && strcmp(tokens[pos].value, "while") == 0)
    {
        while_statement();
    }
    else if (tokens[pos].type == KEYWORD && strcmp(tokens[pos].value, "return") == 0)
    {
        return_statement();
    }
    else if (tokens[pos].type == IDENTIFIER)
    {
        expression_statement();
    }

    printf("]");
}

void if_statement()
{
    printf("IF->[");

    match("if"); // consume 'if' and go to the next token

    if (!strcmp(tokens[pos].value, "(") == 0)
    {
        printf("\n\nSyntax error ❌: expected '(', found %s\n", tokens[pos].value);
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }

    expression();

    block();

    if (tokens[pos].type == KEYWORD && strcmp(tokens[pos].value, "else") == 0)
    {
        match("else"); // consume 'else' and go to the next token
        block();
    }

    printf("]"); 
}

void while_statement()
{
    printf("WHILE->[");

    match("while"); // consume 'while' and go to the next token

    if (tokens[pos].type != SYMBOL || strcmp(tokens[pos].value, "(") != 0)
    {
        printf("\n\nSyntax error ❌: expected '(', found %s\n", tokens[pos].value);
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }

    expression();

    block();

    printf("]");
}

void return_statement()
{
    printf("RETURN->[");

    if (tokens[pos].type == KEYWORD && tokens[pos + 1].type == IDENTIFIER && strcmp(tokens[pos + 2].value, "=") == 0)
    {
        // e.g return sum=2;
        match("return"); // consume return and go to the next token
        assignment();
    }

    else if (tokens[pos].type == KEYWORD && strcmp(tokens[pos + 1].value, "(") == 0) // e.g return (0);
    {
        // return with brackets e.g
        // return (0);
        // return (n(n*2)/3);
        match("return"); // consume return and go to the next token
        match("(");      // consume '(' and go to the next token

        if (!strcmp(tokens[pos].value, ")") == 0)
            expression();
        if (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, "(") == 0)
        {
            expression();
        }

        if (!strcmp(tokens[pos].value, ")") == 0)
        {
            printf("\n\n❌ Syntax ERROR! Closing parenthesis expected.\n\n");
            printf("\n🚫 Parser FAILURE!\n\n");
            exit(EXIT_FAILURE);
        }
        match(")"); // match ')'
    }
    else
    {
        // return without brackets
        // return 0;
        // return n(n*2)/3;
        match("return"); // consume return and go to the next token
        expression();
        if (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, "(") == 0)
        {
            expression();
        }

        if (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, ")") == 0)
            if (!strcmp(tokens[pos].value, ")") == 0)
            {
                printf("\n\n❌ Syntax ERROR! Closing parenthesis expected.\n\n");
                printf("\n🚫 Parser FAILURE!\n\n");
                exit(EXIT_FAILURE);
            }
        match(")"); // match ')
    }
    if (!strcmp(tokens[pos].value, ";") == 0)
    {
        printf("\n\n❌ Syntax ERROR! Expecting a ';'.\n\n");
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }

    match(";"); // consume ';' and go to the next token

    printf("]");
}

void expression_statement()
{
    printf("EXPR_STATEMENT->[");

    if (tokens[pos].type == IDENTIFIER && strcmp(tokens[pos + 1].value, "=") == 0)
    {
        // e.g sum = 0;
        assignment();
    }
    else if (tokens[pos].type == IDENTIFIER && strcmp(tokens[pos + 1].value, "(") == 0)
    {
        // e.g printf();
        identifier(); // consume identifier and go to the next token
        match("(");   // consume '(' and go to the next token

        if (!strcmp(tokens[pos].value, ")") == 0)
        {
            expression();
        }

        if (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, "(") == 0)
        {
            // to deal with nested brackets
            expression();
        }

        while (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, ",") == 0)
        {
            // e.g scanf("Enter a value: %d", &input);

            match(","); // consume ',' and go to the next token
            if (strcmp(tokens[pos].value, "\"") == 0 || tokens[pos].type == STRING)
            {
                printf("\n\n❌ Syntax ERROR! String or '\"' not expected. Expected id, num or symbol, found %s\n", tokens[pos].value);
                printf("\n🚫 Parser FAILURE!\n\n");
                exit(EXIT_FAILURE);
            }
            else if (strcmp(tokens[pos].value, "&") == 0 && tokens[pos + 1].type == IDENTIFIER)
            {
                match("&"); // consume '&' and go the next token
                expression();
            }
            else if (tokens[pos].type == IDENTIFIER && strcmp(tokens[pos + 1].value, "(") == 0)
            {
                // e.g printf("%d", n + sum(n - 1)); | to deal with nested brackets after the comma
                expression();
                if (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, "(") == 0)
                    expression();
            }
            else
                expression();
        }

        if (!strcmp(tokens[pos].value, ")") == 0)
        {
            printf("\n\n❌ Syntax ERROR! Closing parenthesis expected.\n\n");
            printf("\n🚫 Parser FAILURE!\n\n");
            exit(EXIT_FAILURE);
        }
        match(")"); // match ')'
    }
    else
    {
        expression();
    }
    if (!strcmp(tokens[pos].value, ";") == 0)
    {
        printf("\n\n❌ Syntax ERROR! Expecting a ';'.\n\n");
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }

    match(";"); // consume ';' and go to the next token

    printf("]");
}

// Expressions are built from the inmost function - primary() - to the outermost -expression() in a recursive manner
// i.e going inwards upto the center, then building upwards while recursively going inwards again to comb through every function for matching tokens

// function to parse an expression starts from here till primary()
void expression()
{
    printf("EXPRESSION->[");

    logical_or();

    printf("]");
}

// logical OR operation
void logical_or()
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
void logical_and()
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
void equality()
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
void comparison()
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
void assignment()
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
void term()
{
    printf("TERM->[");

    factor();
    while (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "+") == 0 || strcmp(tokens[pos].value, "-") == 0))
    {
        if (strcmp(tokens[pos].value, "+") == 0)
            match("+"); // consume '+' and go to the next token
        else if (strcmp(tokens[pos].value, "-") == 0)
            match("-"); // consume '-' and go to the next token

        factor();
    }

    printf("]");
}

// factor operation
void factor()
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
void unary()
{
    printf("UNARY->[");

    if (tokens[pos].type == OPERATOR && (strcmp(tokens[pos].value, "-") == 0 || strcmp(tokens[pos].value, "!") == 0))
    {
        if (strcmp(tokens[pos].value, "-") == 0)
            match("-"); // consume '-' and go to the next token
        else if (strcmp(tokens[pos].value, "!") == 0)
            match("!"); // consume '!' and go to the next token
    }
    primary();

    printf("]");
}

// primary operation
void primary()
{
    printf("PRIMARY->[");

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
    else if (tokens[pos].type == SYMBOL && strcmp(tokens[pos].value, "(") == 0)
    {
        match("("); // consume '()' and go to the next token
        if (!strcmp(tokens[pos].value, ")") == 0)
            expression();

        if (!strcmp(tokens[pos].value, ")") == 0)
        {
            printf("\n\n❌ Syntax ERROR! Closing parenthesis expected.\n\n");
            printf("\n🚫 Parser FAILURE!\n\n");
            exit(EXIT_FAILURE);
        }
        match(")");
    }
    else
    {
        printf("\n\n❌ Syntax ERROR! Invalid token: %s\n\n", tokens[pos].value);
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }

    printf("]");
}
// *************************************************************************************

void type_specifier()
{
   if (!(strcmp(tokens[pos].value, "int") == 0 || strcmp(tokens[pos].value, "void") == 0 || strcmp(tokens[pos].value, "char") == 0 || strcmp(tokens[pos].value, "float") == 0))
    {
        printf("\n\n❌ Syntax ERROR! Expected type-specifier\n\n");
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }
    printf("TYPE-SPECIFIER->['%s']", tokens[pos].value); // printing the terminal node to the parser tree  
    pos++;
}

void identifier()
{
    if (tokens[pos].type != IDENTIFIER)
    {
        printf("\n\n❌ Syntax ERROR! Expected identifier, but got %s.\n\n", tokens[pos].value);
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }
    printf("IDENTIFIER->['%s']", tokens[pos].value); // printing the terminal node adding it to the parser tree   
    pos++;
}

void number()
{
    if (tokens[pos].type != NUMBER)
    {
        printf("\n\n❌ Syntax ERROR! Expected number, but got %s.\n\n", tokens[pos].value);
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }
    printf("NUMBER->['%s']", tokens[pos].value); // printing the terminal node adding it to the parser tree  
    pos++;
}

void string()
{
    if (tokens[pos].type != STRING)
    {
        printf("\n\n❌ Syntax ERROR! Expected string, but got %s.\n\n", tokens[pos].value);
        printf("\n🚫 Parser FAILURE!\n\n");
        exit(EXIT_FAILURE);
    }
    printf("STRING->['%s']", tokens[pos].value);
    pos++;
}

void match(char *value)
{
    if (strcmp(tokens[pos].value, value) == 0)
    {
        switch (tokens[pos].type)
        {
        case KEYWORD:
            if (tokens[pos].type != KEYWORD) // e.g match("if")
            {
                printf("\n\n❌ Syntax ERROR! Expected keyword, but got %s.\n\n", tokens[pos].value);
                printf("\n🚫 Parser FAILURE!\n\n");
                exit(EXIT_FAILURE);
            }
            printf("keyword->['%s']", tokens[pos].value);

            pos++;
            break;
        case OPERATOR:
            if (tokens[pos].type != OPERATOR) // e.g match("+")
            {
                printf("\n\n❌ Syntax ERROR! Expected operator, but got %s.\n\n", tokens[pos].value);
                printf("\n🚫 Parser FAILURE!\n\n");
                exit(EXIT_FAILURE);
            }
            printf("operator->['%s']", tokens[pos].value);
            pos++;
            break;
        case SYMBOL:
            if (tokens[pos].type != SYMBOL) // e.g match("{")
            {
                printf("\n\n❌ Syntax ERROR! Expected symbol, but got %s.\n\n", tokens[pos].value);
                printf("\n🚫 Parser FAILURE!\n\n");
                exit(EXIT_FAILURE);
            }
            printf("symbol->['%s']", tokens[pos].value);
            pos++;
            break;
        default:
            break;
        }
    }
}
