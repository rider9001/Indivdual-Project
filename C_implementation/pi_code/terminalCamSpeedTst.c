#include <stdlib.h>
#include <stdio.h>

int main()
{
    system("raspistill -o out.jpg");

    printf("Complete\n");

    return 0;
}
