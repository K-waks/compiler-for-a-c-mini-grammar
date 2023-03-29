/* Group 11 CSC 326 sample program
A program to find the sum of numbers from 0 till the number inputed by the user
*/

#include <stdio.h>

int sum(int n)
{
    if (n <= 1)
    {
        return 1;
    }
    else
    {
        return (n + sum(n - 1));
    }
}

int main()
{
    int n;
    printf("Enter an integer: ");
    scanf("%d", &n);
    printf("Sum of numbers from 0 till %d is %d.", n, sum(n));
    return (0);
}