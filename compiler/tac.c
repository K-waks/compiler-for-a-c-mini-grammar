#include "parser.c"

int label_count = 0;
int temp_count = 0;
char last_temp_var[10] = "";
char temp[10];

int new_label()
{
    return ++label_count;
}

char *last_temp()
{
    return last_temp_var;
}

/* Traverse the parse tree and generate three-address code for each node */
void generate_code(Node *node)
{
    if (node == NULL)
    {
        return;
    }
    else
    {
        switch (node->type)
        {

        case PROGRAM:
            for (int i = 0; i < node->num_children; i++)
            {
                generate_code(node->children[i]);
            }
            break;

        case FUNCTION_DECLARATION:
            printf("%s:\n", node->children[1]->value); // Print function label

            if (strcmp(node->children[4]->value, "BLOCK") == 0)
            {
                generate_code(node->children[4]);
            }
            else if (strcmp(node->children[5]->value, "BLOCK") == 0)
            {
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
        {
            int L1 = new_label();
            int L2 = new_label();

            generate_code(node->children[2]); // Generate code for the condition

            strcpy(temp, last_temp());                    // Store the variable result of the condition temporarily
            sprintf(last_temp_var, "t%d", ++temp_count);  // Create new temporary variable
            printf("NOT, %s, %s\n", last_temp_var, temp); // store the negation of the condition result in the new variable
            printf("IF, %s, L%d\n", last_temp_var, L1);   // Jump to L1 if condition is false
            generate_code(node->children[4]);             // Generate code for the if-block
            printf("GOTO, L%d\n", L2);                    // Jump to L2 after if-block
            printf("Label, L%d\n", L1);                   // Define label L1 for else-block
            generate_code(node->children[6]);             // Generate code for the else-block
            printf("Label, L%d\n", L2);                   // Define label L2 after if-else statement
            break;
        }

        case WHILE_STATEMENT:
        {
            int L1 = new_label();
            int L2 = new_label();

            printf("Label, L%d\n", L1);               // Define label L1 for loop start
            generate_code(node->children[2]);         // Generate code for the condition
            printf("IF, %s, L%d\n", last_temp(), L2); // Jump to L2 if condition is false
            generate_code(node->children[4]);         // Generate code for the loop body
            printf("GOTO, L%d\n", L1);                // Jump back to L1 for next iteration
            printf("Label, L%d\n", L2);               // Define label L2 after loop
            break;
        }

        case RETURN_STATEMENT:
            generate_code(node->children[0]);    // Generate code for the return value
            printf("RETURN, %s\n", last_temp()); // Return the value
            break;

        case EXPRESSION_STATEMENT:
        {
            if (node->num_children == 1)
            {
                generate_code(node->children[0]);
                // No need to store the result in a temporary variable, so don't do anything
            }
            else if (node->num_children == 2 && strcmp(node->children[0]->value, "-") == 0)
            {
                generate_code(node->children[1]);
                sprintf(last_temp_var, "t%d", ++temp_count);
                printf("SUB, %s, 0, %s\n", last_temp_var, last_temp()); // Negate the value and store it in the temporary variable
            }
            else if (node->num_children == 3 && strcmp(node->children[1]->value, "+") == 0)
            {
                generate_code(node->children[0]);                                   // Generate code for the left-hand side of the addition
                char *left_temp = last_temp();                                      // Store the temporary variable holding the left-hand side
                generate_code(node->children[2]);                                   // Generate code for the right-hand side of the addition
                sprintf(last_temp_var, "t%d", ++temp_count);                        // Create new temporary variable
                printf("ADD, %s, %s, %s\n", last_temp_var, left_temp, last_temp()); // Add the left-hand side and right-hand side and store it in the temporary variable
            }
            else if (node->num_children == 3 && strcmp(node->children[1]->value, "-") == 0)
            {
                generate_code(node->children[0]);                                   // Generate code for the left-hand side of the subtraction
                char *left_temp = last_temp();                                      // Store the temporary variable holding the left-hand side
                generate_code(node->children[2]);                                   // Generate code for the right-hand side of the subtraction
                sprintf(last_temp_var, "t%d", ++temp_count);                        // Create new temporary variable
                printf("SUB, %s, %s, %s\n", last_temp_var, left_temp, last_temp()); // Subtract the right-hand side from the left-hand side and store it in the temporary variable
            }
            else if (node->num_children == 3 && strcmp(node->children[1]->value, "*") == 0)
            {
                generate_code(node->children[0]);                                   // Generate code for the left-hand side of the multiplication
                char *left_temp = last_temp();                                      // Store the temporary variable holding the left-hand side
                generate_code(node->children[2]);                                   // Generate code for the right-hand side of the multiplication
                sprintf(last_temp_var, "t%d", ++temp_count);                        // Create new temporary variable
                printf("MUL, %s, %s, %s\n", last_temp_var, left_temp, last_temp()); // Multiply the left-hand side and right-hand side and store it in the temporary variable
            }
            else if (node->num_children == 3 && strcmp(node->children[1]->value, "/") == 0)
            {
                generate_code(node->children[0]);                                   // Generate code for the left-hand side of the division
                char *left_temp = last_temp();                                      // Store the temporary variable holding the left-hand side
                generate_code(node->children[2]);                                   // Generate code for the right-hand side ofthe division
                sprintf(last_temp_var, "t%d", ++temp_count);                        // Create new temporary variable
                printf("DIV, %s, %s, %s\n", last_temp_var, left_temp, last_temp()); // Divide the left-hand side by the right-hand side and store it in the temporary variable
            }
            else
            {
                fprintf(stderr, "Error: Invalid expression statement.\n");
                exit(EXIT_FAILURE);
            }
            break;
        }

        case TERMINAL:

            if (strcmp(node->value, "Unary") == 0)
            {
                generate_code(node->children[1]);
                printf("SUB, %s, 0, %s\n", last_temp_var, last_temp()); // Negate the value and store it in the temporary variable

                strcpy(temp, last_temp_var);                     // Store the temporary variable holding the negation
                sprintf(last_temp_var, "t%d", ++temp_count);     // Create another new temporary variable
                printf("ASSIGN, %s, %s\n", last_temp_var, temp); // Assign negation value to the latter temporary variable
            }
            else
            {
                sprintf(last_temp_var, "t%d", ++temp_count);            // Create new temporary variable
                printf("ASSIGN, %s, %s\n", last_temp_var, node->value); // Assign terminal value to temporary variable
            }
            break;
        default:
            fprintf(stderr, "Error: Unknown node type: %d\n", node->type);
            exit(1);
        }
    }
}
