void main()
{
    int x;
    int y;

    x = 0;
    y = 5;

    if (y > x)
    {
        printf("y is greater than x\n\n");
    }
    else
    {
        printf("x is greater than y\n\n");
    }

    while (x < y)
    {
        printf("This will loop five times\n");
        x = x + 1;
    }
}
