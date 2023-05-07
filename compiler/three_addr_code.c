#include "parser.c"

int temp_counter = 0; // counter for generating temporary variable names

/* Helper function to generate a new temporary variable name */
char *new_temp()
{
    char *temp_name = (char *)malloc(sizeof(char) * 10);
    snprintf(temp_name, 10, "t%d", temp_counter++);
    return temp_name;
}

/* Traverse the parse tree and generate three-address code for each node */
void generate_code(Node *node)
{
    if (node == NULL)
    {
        return;
    }
    // Generate code for each non-terminal node based on its type and children
    if (strcmp(node->value, "PROGRAM") == 0)
    {
        for (int i = 0; i < node->num_children; i++)
        {
            generate_code(node->children[i]);
        }
    }

    else if (strcmp(node->value, "FUNCTION DECLARATION") == 0)
    { // int sum(){} 
        printf("%s:\n", node->children[1]->value); // Print function label
        generate_code(node->children[3]);          // Generate code for block
        printf("RETURN\n");                        // Return from function
    }

    else if (strcmp(node->value, "BLOCK") == 0)
    {
        // Generate code for block
        for (int i = 1; i < node->num_children - 1; i++)
        {
            generate_code(node->children[i]);
        }
    }
    else if (strcmp(node->value, "VARIABLE DECLARATION") == 0)
    {
        // Generate code for variable declaration
        // Do nothing, since no code is required for variable declaration
    }
    else if (strcmp(node->value, "PARAMETER DECLARATION") == 0)
    {
        // Generate code for parameter declaration
        // Do nothing, since no code is required for parameter declaration
    }
    else if (strcmp(node->value, "STATEMENT") == 0)
    {
        if (strcmp(node->children[0]->value, "IF STATEMENT") == 0)
        {
            // Generate code for if statement
            char *temp = new_temp();                    // Generate a new temporary variable name
            generate_code(node->children[1]);           // Generate code for condition expression
            printf("BZ %s, L%d\n", temp, temp_counter); // Branch to else label if condition is false
            generate_code(node->children[2]);           // Generate code for if block
            printf("JMP L%d\n", temp_counter + 1);      // Jump to end of if statement
            printf("L%d:\n", temp_counter);             // Else label
            generate_code(node->children[3]);           // Generate code for else block
            printf("L%d:\n", temp_counter + 1);         // End of if statement label
            temp_counter += 2;                          // Increment temporary variable counter by 2
        }
        else if (strcmp(node->children[0]->value, "WHILE STATEMENT") == 0)
        {
            // Generate code for while statement
            printf("L%d:\n", temp_counter);                 // Loop label
            char *temp = new_temp();                        // Generate a new temporary variable name
            generate_code(node->children[1]);               // Generate code for condition expression
            printf("BZ %s, L%d\n", temp, temp_counter + 1); // Branch to end label if condition is false
            generate_code(node->children[2]);               // Generate code for while block
            printf("JMP L%d\n", temp_counter);              //
        }
        else if (strcmp(node->value, "EXPRESSION") == 0)
        {
            // Generate code for expression
            if (node->num_children == 1)
            {
                // Generate code for single expression
                generate_code(node->children[0]);
            }
            else
            {
                // Generate code for binary expression
                char *temp1 = new_temp();                                // Generate a new temporary variable name
                generate_code(node->children[0]);                        // Generate code for left operand
                printf("MOV %s, %s\n", temp1, node->children[0]->value); // Move result to temporary variable
                for (int i = 1; i < node->num_children; i += 2)
                {
                    char *temp2 = new_temp();             // Generate a new temporary variable name
                    generate_code(node->children[i + 1]); // Generate code for right operand
                    if (strcmp(node->children[i]->value, "+") == 0)
                    {
                        printf("ADD %s, %s, %s\n", temp1, temp1, node->children[i + 1]->value); // Add operands
                    }
                    else if (strcmp(node->children[i]->value, "-") == 0)
                    {
                        printf("SUB %s, %s, %s\n", temp1, temp1, node->children[i + 1]->value); // Subtract operands
                    }
                    else if (strcmp(node->children[i]->value, "*") == 0)
                    {
                        printf("MUL %s, %s, %s\n", temp1, temp1, node->children[i + 1]->value); // Multiply operands
                    }
                    else if (strcmp(node->children[i]->value, "/") == 0)
                    {
                        printf("DIV %s, %s, %s\n", temp1, temp1, node->children[i + 1]->value); // Divide operands
                    }
                    printf("MOV %s, %s\n", temp2, temp1); // Move result to temporary variable
                }
                node->value = temp1; // Save result in value attribute of expression node
            }
        }
        else if (strcmp(node->value, "IDENTIFIER") == 0)
        {
            // Generate code for identifier
            node->value = node->children[0]->value; // Save identifier name in value attribute of identifier node
        }
        else if (strcmp(node->value, "NUMBER") == 0)
        {
            // Generate code for number
            char *temp = new_temp();                                // Generate a new temporary variable name
            printf("MOV %s, %s\n", temp, node->children[0]->value); // Move number value to temporary variable
            node->value = temp;                                     // Save temporary variable name in value attribute of number node
        }
        else
        {
            printf("Error: unrecognized node type \"%s\"\n", node->value); // Error message for unrecognized node type
        }
    }
}