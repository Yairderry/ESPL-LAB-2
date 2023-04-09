#include "linux/limits.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "LineParser.h"
#include "string.h"

void execute(cmdLine *cmdLine);
void printError(char *errorMessage);

int main(int argc, char const *argv[])
{
    char cwd[PATH_MAX];
    char line[2048];
    cmdLine *cmdLine;
    getcwd(cwd, PATH_MAX);

    while (1)
    {
        printf("the current working directory is: %s\n", cwd);
        if (fgets(line, sizeof(line), stdin) == NULL)
            printError("Line Reading Error");

        if (strncmp(line, "quit\n", 4) == 0)
            exit(0);

        cmdLine = parseCmdLines(line);

        if (cmdLine == NULL)
            printError("Parsing Error");

        execute(cmdLine);
        freeCmdLines(cmdLine);
    }
    return 0;
}

void execute(cmdLine *cmdLine)
{
    if (execvp(cmdLine->arguments[0], cmdLine->arguments) == -1)
        printError("Execution Error");
}

void printError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}
