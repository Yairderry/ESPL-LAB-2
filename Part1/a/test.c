#include "stdio.h"

int main(int argc, char const *argv[])
{
    char line[2048];
    printf("lol hey\n");
    for (int i = 1; i < argc; i++)
        printf("%d) %s\n", i, argv[i]);
    if (fgets(line, sizeof(line), stdin) != NULL)
        printf("Echo: %s\n",line);
    return 0;
}
