// void match_equals(enum Token expected_token){
//     if ((current_token == expected_token) && )
// }

// void match(enum Token expected_token)
// {
//     if (current_token == expected_token)
//     {
//         scanf("%d", &store);
//         current_token = (enum Token)store;

//         // current_token = get_token(stdin, token_buffer);
//     }
//     else if (expected_token == SYMBOL && current_token >= SYMBOL_START && current_token <= SYMBOL_END)
//     {
//         scanf("%d", &store);
//         current_token = (enum Token)store;

//         // current_token = get_token(stdin, token_buffer);
//     }
//     else
//     {
//         error("unexpected token");
//     }
// }



scanf("%s", store);
    if (strcmp(store, 'IDENTIFIER') == 0)
    {
        current_token = 0;
    }
    else if (strcmp(store, 'KEYWORD') == 0)
    {
        current_token = 1;
    }
    else if (strcmp(store, 'OPERATOR') == 0)
    {
        current_token = 2;
    }
    else if (strcmp(store, 'NUMBER') == 0)
    {
        current_token = 3;
    }
    else if (strcmp(store, 'STRING') == 0)
    {
        current_token = 4;
    }
    else if (strcmp(store, 'COMMENT') == 0)
    {
        current_token = 5;
    }
    else if (strcmp(store, 'NEWLINE') == 0)
    {
        current_token = 6;
    }
    else if (strcmp(store, 'SYMBOL') == 0)
    {
        current_token = 7;
    }
    else if (strcmp(store, 'ENDOFFILE') == 0)
    {
        current_token = 8;
    }
    scanf("%s", token_buffer);

    // current_token = get_token(stdin, token_buffer);

    program();

    if (current_token == ENDOFFILE)
        printf("Parsing successful");
