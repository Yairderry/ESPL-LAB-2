#include "linux/limits.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/wait.h"
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

    printf("the current working directory is: %s\n", cwd);

    // Debug mode
    for (int i = 1; i < argc; i++)
        if (argv[i][0] == '-' && argv[i][1] == 'd')
            fprintf(stderr, "PID: %d\nExecuting command: %s\n", getpid(), argv[0]);

    while (1)
    {
        if (fgets(line, sizeof(line), stdin) == NULL)
            printError("Line Reading Error");

        if (strncmp(line, "quit\n", 5) == 0)
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
    pid_t pid = fork();
    if (pid < 0)
        printError("Fork Error");
    else if (pid == 0)
    {
        if (execvp(cmdLine->arguments[0], cmdLine->arguments) == -1)
            printError("Execution Error");
    }
    else
        wait(NULL);
}

void printError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}
