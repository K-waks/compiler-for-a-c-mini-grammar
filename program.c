int sum(int n)
{
    if (n <= 1)
    {
        return (1);
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

// if (232 a3 >= "ast" || 23.5 <= 90.s3) do {a2 = 89}

