enum Token_Type get_token(FILE *fp, char *buffer)
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
                printf("\n\n❌ Lexer ERROR! Invalid token: %s%c\n\n", buffer, c);
                return INVALID_TOKEN;
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
                printf("\n\n❌ Lexer ERROR! Invalid token: %s\n\n", buffer);
                return INVALID_TOKEN;
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
            buffer[buffer_len++] = c;
            printf("\n\n❌ Lexer ERROR! Unknown token: %s\n\n", buffer);
            ungetc(c, fp);
            return INVALID_TOKEN;
        }
    }
    return EOF;
}

int lexer()
{
    // file descriptor variable declared and buffer of dynamic size to hold the read characters
    FILE *fp;
    int c;
    char buffer[BUFSIZ];

    // opening the file for scanning
    fp = fopen("program.c", "r");
    if (fp == NULL)
    {
        printf("\n\n❌ Lexer ERROR! Can't open file for scanning.\n\n");
        return EXIT_FAILURE;
    }
    else if ((c = fgetc(fp)) == EOF)
    {
        printf("\n\n❌ Lexer ERROR! File is empty.\n\n");
        return EXIT_FAILURE;
    }
    ungetc(c, fp);
    

    // saving each token to the struct array while printing to the terminal
    enum Token_Type token_type;
    int token_count = 0;

    while ((token_type = get_token(fp, buffer)) != EOF)
    {
        if (token_type == INVALID_TOKEN)
        {
            return EXIT_FAILURE;
        }

        // printing to the terminal
        switch (token_type)
        {
        case KEYWORD:
            printf("KEYWORD: %s\n", buffer);
            break;
        case IDENTIFIER:
            printf("IDENTIFIER: %s\n", buffer);
            break;
        case NUMBER:
            printf("NUMBER: %s\n", buffer);
            break;
        case STRING:
            printf("STRING: %s\n", buffer);
            break;
        case OPERATOR:
            printf("OPERATOR: %s\n", buffer);
            break;
        case SYMBOL:
            printf("SYMBOL: %s\n", buffer);
            break;
        default:
            break;
        }
        struct Token current_tkn;
        current_tkn.type = token_type;
        strcpy(current_tkn.value, buffer);
        tokens[token_count++] = current_tkn; // adding the token (struct) to our array of structs.
    }

    // adding the terminating symbol to the end of the array that will be used to tell the parser that we've reached the end of parsing
    struct Token terminator;
    terminator.type = EOF;
    strcpy(terminator.value, "$");
    tokens[token_count++] = terminator;

    // closing the file
    fclose(fp);

    return EXIT_SUCCESS;
}