#include "stdio.h"

int main(int argc, char const *argv[])
{
    printf("lol hey\n");
    for (int i = 1; i < argc; i++)
        printf("%d) %s\n", i, argv[i]);
    return 0;
}
