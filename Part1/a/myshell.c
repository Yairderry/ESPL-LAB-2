#include "linux/limits.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/wait.h"
#include "LineParser.h"
#include "string.h"

void execute(cmdLine *cmdLine);
void error(char *errorMessage);

int main(int argc, char const *argv[])
{
    char cwd[PATH_MAX];
    char line[2048];
    cmdLine *cmdLine;
    getcwd(cwd, PATH_MAX);

    // Debug mode
    for (int i = 1; i < argc; i++)
        if (argv[i][0] == '-' && argv[i][1] == 'd')
            fprintf(stderr, "PID: %d\nExecuting command: %s\n", getpid(), argv[0]);

    while (1)
    {
        printf("the current working directory is: %s\n", cwd);

        if (fgets(line, sizeof(line), stdin) == NULL)
            error("Line Reading Error");

        if (strncmp(line, "quit\n", 5) == 0)
            exit(0);

        cmdLine = parseCmdLines(line);

        if (cmdLine == NULL)
            error("Parsing Error");

        execute(cmdLine);
        freeCmdLines(cmdLine);
    }
    return 0;
}

void execute(cmdLine *cmdLine)
{
    pid_t pid = fork();
    if (pid < 0)
        error("Fork Error");
    else if (pid == 0)
        if (execvp(cmdLine->arguments[0], cmdLine->arguments) == -1)
            error("Execution Error");
}

void error(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}
