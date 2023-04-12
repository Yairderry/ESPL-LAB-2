#include "linux/limits.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/wait.h"
#include "LineParser.h"
#include "string.h"
#include "sys/types.h"
#include "signal.h"
#include "sys/stat.h"
#include "fcntl.h"

void execute(cmdLine *cmdLine, int debug);
void error(char *errorMessage);
void inputRedirect(char const *path);
void outputRedirect(char const *path);

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
            exit(EXIT_SUCCESS);

        cmdLine = parseCmdLines(line);

        if (cmdLine == NULL)
            error("Parsing Error");

        execute(cmdLine, debug);
        freeCmdLines(cmdLine);
    }
    return 0;
}

void execute(cmdLine *cmdLine, int debug)
{
    int isCD = cmdLine->arguments[0][0] == 'c' && cmdLine->arguments[0][1] == 'd';

    if (isCD && chdir(cmdLine->arguments[1]) < 0)
        error("Changing Directories Error");
    else if (isCD)
        return;

    pid_t pid = fork();

    // Couldn't fork for any reason
    if (pid < 0)
        error("Fork Error");

    // Parent process
    if (pid > 0 && debug)
        fprintf(stderr, "PID: %d\nExecuting command: %s\n", pid, cmdLine->arguments[0]);
    if (pid > 0 && cmdLine->blocking)
        waitpid(pid, NULL, 0);

    // Child process
    if (pid == 0 && cmdLine->inputRedirect)
        inputRedirect(cmdLine->inputRedirect);
    if (pid == 0 && cmdLine->outputRedirect)
        outputRedirect(cmdLine->outputRedirect);
    if (pid == 0 && execvp(cmdLine->arguments[0], cmdLine->arguments) == -1)
        error("Execution Error");
}

void error(char *errorMessage)
{
    perror(errorMessage);
    exit(EXIT_FAILURE);
}

void inputRedirect(char const *path)
{
    int fileDescriptor = open(path, O_RDONLY);

    if (fileDescriptor < 0)
        error("Redirect Error");

    close(STDIN_FILENO);

    if (dup(fileDescriptor) < 0)
        error("Duplication Error");

    close(fileDescriptor);
}

void outputRedirect(char const *path)
{
    int fileDescriptor = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fileDescriptor < 0)
        error("Redirect Error");

    close(STDOUT_FILENO);

    if (dup(fileDescriptor) < 0)
        error("Duplication Error");

    close(fileDescriptor);
}
