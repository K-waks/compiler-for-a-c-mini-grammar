#include "parser.c"

int label_count = 0;
int temp_count = 0;
char temp_var[10] = "";

int new_label()
{
    return ++label_count;
}

void new_temp()
{
    sprintf(temp_var, "t%d", ++temp_count);
}

/* Traverse the parse tree and generate three-address code for each node */
void generate_code(Node *node)
{
    int L1;
    int L2;
    char temp[10];
    char left_temp[10];
    char right_temp[10];

    switch (node->type)
    {

    case PROGRAM:

        for (int i = 0; i < node->num_children; i++)
        {
            generate_code(node->children[i]);
        }

        break;

    case FUNCTION_DECLARATION:

        printf("\n%s:\n", node->children[1]->value); // Print function label

        if (strcmp(node->children[4]->value, "BLOCK") == 0)
        { // to take account of functions which don't have parameters e.g int sum(){}
            generate_code(node->children[4]);
        }
        else if (strcmp(node->children[5]->value, "BLOCK") == 0)
        { // function with parameters e.g int sum(int n){}
            generate_code(node->children[5]);
        }

        break;

    case PARAMETER_DECLARATION:
        // Parameter declarations don't generate code

        break;

    case VARIABLE_DECLARATION:
        // Variable declarations don't generate code

        break;

    case BLOCK:

        for (int i = 1; i < node->num_children - 1; i++)
        {
            generate_code(node->children[i]);
        }

        break;

    case IF_STATEMENT:

        L1 = new_label();
        L2 = new_label();

        generate_code(node->children[2]);        // Generate code for the condition
        strcpy(temp, temp_var);                  // Store the variable result of the condition temporarily
        new_temp();                              // Create new temporary variable
        printf("NOT, %s, %s\n", temp_var, temp); // store the negation of the condition result in the new variable
        printf("IF, %s, L%d\n", temp_var, L1);   // Jump to L1 if condition is false
        generate_code(node->children[4]);        // Generate code for the if-block
        printf("GOTO, L%d\n", L2);               // Jump to L2 after if-block
        printf("Label, L%d\n", L1);              // Define label L1 for else-block
        generate_code(node->children[6]);        // Generate code for the else-block
        printf("Label, L%d\n", L2);              // Define label L2 after if-else statement

        break;

    case WHILE_STATEMENT:

        L1 = new_label();
        L2 = new_label();
        
        printf("Label, L%d\n", L1);              // Define label L1 for loop start
        generate_code(node->children[2]);        // Generate code for the condition
        strcpy(temp, temp_var);                  // Store the variable result of the condition temporarily
        new_temp();                              // Create new temporary variable
        printf("NOT, %s, %s\n", temp_var, temp); // store the negation of the condition result in the new variable
        printf("IF, %s, L%d\n", temp_var, L2);   // Jump to L2 if condition is false
        generate_code(node->children[4]);        // Generate code for the loop body
        printf("GOTO, L%d\n", L1);               // Jump back to L1 for next iteration
        printf("Label, L%d\n", L2);              // Define label L2 after loop

        break;

    case RETURN_STATEMENT:

        generate_code(node->children[2]); // Generate code for the return value
        printf("RETURN, %s\n", temp_var); // Return the value

        break;

    case PRINTF_STATEMENT:

        generate_code(node->children[2]); // Generate code for the argument
        printf("PRINT, %s\n", temp_var);  // Print the value of the argument

        break;

    case SCANF_STATEMENT:

        generate_code(node->children[2]); // Generate code for the argument
        printf("SCAN, %s\n", temp_var);   // Read input and store it in the variable

        break;

    case ASSIGNMENT_STATEMENT:

        generate_code(node->children[0]->children[0]);     // Generate code for the left-hand side of the addition
        strcpy(left_temp, temp_var);                       // Store the temporary variable holding the left-hand side
        generate_code(node->children[0]->children[1]);     // Generate code for the right-hand side of the addition
        strcpy(right_temp, temp_var);                      // Store the temporary variable holding the right-hand side
        printf("ASSIGN, %s, %s\n", left_temp, right_temp); // Store the right hand side into the left-hand side

        break;

    case FUNCTION_CALL:

        char *function_name = node->children[0]->value; // Get the name of the function being called

        if (strcmp(node->children[2]->value, ")") != 0)
        {
            for (int i = 2; i < node->num_children; i = i + 2) // Generate code for each argument
            {
                generate_code(node->children[i]);
                printf("ARG, %s\n", temp_var); // Pass the argument to the function
            }
        }

        new_temp();                                                      // Create new temporary variable to hold the function result
        printf("CALL, %s, %d\n", function_name, node->num_children - 4); // Call the function with the correct number of arguments
        printf("MOVE, %s, RET\n", temp_var);                             // Move the function result to the temporary variable

        break;

    case EXPRESSION:

        if (node->num_children == 2 && strcmp(node->children[0]->value, "-") == 0)
        {
            generate_code(node->children[1]);
            new_temp();
            printf("SUB, %s, 0, %s\n", temp_var, temp_var); // Negate the value and store it in the temporary variable
        }
        else if (node->num_children == 1 && (strcmp(node->children[0]->value, "+") == 0 ||
                                             strcmp(node->children[0]->value, "-") == 0 ||
                                             strcmp(node->children[0]->value, "*") == 0 ||
                                             strcmp(node->children[0]->value, "/") == 0 ||
                                             strcmp(node->children[0]->value, "%") == 0 ||
                                             strcmp(node->children[0]->value, "<") == 0 ||
                                             strcmp(node->children[0]->value, ">") == 0 ||
                                             strcmp(node->children[0]->value, "==") == 0 ||
                                             strcmp(node->children[0]->value, "!=") == 0 ||
                                             strcmp(node->children[0]->value, "<=") == 0 ||
                                             strcmp(node->children[0]->value, ">=") == 0 ||
                                             strcmp(node->children[0]->value, "&&") == 0 ||
                                             strcmp(node->children[0]->value, "||") == 0 ||
                                             strcmp(node->children[0]->value, "=") == 0))
        {
            generate_code(node->children[0]->children[0]); // Generate code for the left-hand side of the addition
            strcpy(left_temp, temp_var);                   // Store the temporary variable holding the left-hand side
            generate_code(node->children[0]->children[1]); // Generate code for the right-hand side of the addition
            strcpy(right_temp, temp_var);                  // Store the temporary variable holding the right-hand side
            new_temp();                                    // Create new temporary variable

            if (strcmp(node->children[0]->value, "+") == 0)
            {
                printf("ADD, %s, %s, %s\n", temp_var, left_temp, right_temp); // Add the left-hand side and right-hand side and store it in the temporary variable
            }
            else if (strcmp(node->children[0]->value, "-") == 0)
            {
                printf("SUB, %s, %s, %s\n", temp_var, left_temp, right_temp); // Subtract the left-hand side and right-hand side and store it in the temporary variable
            }
            else if (strcmp(node->children[0]->value, "*") == 0)
            {
                printf("MUL, %s, %s, %s\n", temp_var, left_temp, right_temp); // Multiply the left-hand side and right-hand side and store it in the temporary variable
            }
            else if (strcmp(node->children[0]->value, "/") == 0)
            {
                printf("DIV, %s, %s, %s\n", temp_var, left_temp, right_temp); // Divide the left-hand side and right-hand side and store it in the temporary variable
            }
            else if (strcmp(node->children[0]->value, "%") == 0)
            {
                printf("MOD, %s, %s, %s\n", temp_var, left_temp, right_temp); // Divide the left by right-hand side and store remainder in the temporary variable
            }
            else if (strcmp(node->children[0]->value, "<") == 0)
            {
                printf("LT, %s, %s, %s\n", temp_var, left_temp, right_temp); // Bool of the left-hand side being less than right-hand side and store it in the temp var
            }
            else if (strcmp(node->children[0]->value, ">") == 0)
            {
                printf("GT, %s, %s, %s\n", temp_var, left_temp, right_temp); // Bool of the left-hand side being greater than right-hand side and store it in the temp var
            }
            else if (strcmp(node->children[0]->value, "==") == 0)
            {
                printf("EQ, %s, %s, %s\n", temp_var, left_temp, right_temp); // Bool of the left-hand side being equal to right-hand side and store it in the temp var
            }
            else if (strcmp(node->children[0]->value, "!=") == 0)
            {
                printf("NEQ, %s, %s, %s\n", temp_var, left_temp, right_temp); // Bool of the left-hand side being not equal to right-hand side, store it in the temp var
            }
            else if (strcmp(node->children[0]->value, "<=") == 0)
            {
                printf("LEQ, %s, %s, %s\n", temp_var, left_temp, right_temp); // Bool of the left-hand side being less than or equal to right-hand side and store it in the temp var
            }
            else if (strcmp(node->children[0]->value, ">=") == 0)
            {
                printf("GEQ, %s, %s, %s\n", temp_var, left_temp, right_temp); // Bool of the left-hand side being greater than or equal to right-hand side and store it in the temp var
            }
            else if (strcmp(node->children[0]->value, "&&") == 0)
            {
                printf("AND, %s, %s, %s\n", temp_var, left_temp, right_temp); // Store the AND of the left-hand side and right-hand side in the temp var
            }
            else if (strcmp(node->children[0]->value, "||") == 0)
            {
                printf("OR, %s, %s, %s\n", temp_var, left_temp, right_temp); // Store the OR of the left-hand side and right-hand side in the temp var
            }
            else if (strcmp(node->children[0]->value, "=") == 0)
            {
                printf("ASSIGN, %s, %s, %s\n", temp_var, left_temp, right_temp); // Store the right hand side into the left-hand side
            }
        }
        else if (node->num_children == 1)
        {
            generate_code(node->children[0]);
        }

        else
        {
            fprintf(stderr, "Error: Invalid expression statement.\n");
            exit(EXIT_FAILURE);
        }

        break;

    case TERMINAL:

        new_temp();                                        // Create new temporary variable
        printf("ASSIGN, %s, %s\n", temp_var, node->value); // Assign terminal value to temporary variable

        break;

    case NESTED:

        generate_code(node->children[1]);
        break;

    case UNARY:

        generate_code(node->children[1]);
        strcpy(temp, temp_var);
        new_temp();
        printf("SUB, %s, 0, %s\n", temp_var, temp); // Negate the value and store it in the temporary variable

        break;

    default:

        fprintf(stderr, "Error: Unknown node type: %d\n", node->type);
        exit(1);
    }
}
