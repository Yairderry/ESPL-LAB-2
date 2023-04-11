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
    int debug = 0;

    // Debug mode
    for (int i = 1; i < argc; i++)
        if (argv[i][0] == '-' && argv[i][1] == 'd')
            debug = 1;

    while (1)
    {
        getcwd(cwd, PATH_MAX);
        printf("%s\n", cwd);
        if (fgets(line, sizeof(line), stdin) == NULL)
            error("Line Reading Error");

        if (strncmp(line, "quit\n", 5) == 0)
            exit(0);

        cmdLine = parseCmdLines(line);

        if (cmdLine == NULL)
            error("Parsing Error");

        if (debug)
            fprintf(stderr, "PID: %d\nExecuting command: %s\n", getpid(), argv[0]);

        execute(cmdLine);
        freeCmdLines(cmdLine);
    }
    return 0;
}

void execute(cmdLine *cmdLine)
{
    pid_t pid = fork();
    int status;

    // Couldn't fork for any reason
    if (pid < 0)
        error("Fork Error");
    // Child process but execution failed
    else if (pid == 0 && execvp(cmdLine->arguments[0], cmdLine->arguments) == -1)
        error("Execution Error");
    // Parent process and command is blocking
    else if (pid > 0 && cmdLine->blocking)
        waitpid(pid, &status, 0);
}

void error(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}
