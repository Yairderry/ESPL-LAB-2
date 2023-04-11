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
int signalProcess(char *pid, int signal);
int cd(char *path);

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
        printf("%s ", cwd);
        if (fgets(line, sizeof(line), stdin) == NULL)
            error("Line Reading Error");

        if (strncmp(line, "quit\n", 5) == 0)
            exit(0);

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
    int isBasicCommand = 0;

    if (strcmp(cmdLine->arguments[0], "cd") == 0)
        isBasicCommand = cd(cmdLine->arguments[1]);
    else if (strcmp(cmdLine->arguments[0], "kill") == 0)
        isBasicCommand = signalProcess(cmdLine->arguments[1], SIGTERM);
    else if (strcmp(cmdLine->arguments[0], "wake") == 0)
        isBasicCommand = signalProcess(cmdLine->arguments[1], SIGCONT);
    else if (strcmp(cmdLine->arguments[0], "suspend") == 0)
        isBasicCommand = signalProcess(cmdLine->arguments[1], SIGTSTP);

    if (isBasicCommand)
        return;

    pid_t pid = fork();

    // Print debug to child process
    if (debug && pid > 0)
        fprintf(stderr, "PID: %d\nExecuting command: %s\n", pid, cmdLine->arguments[0]);

    // Couldn't fork for any reason
    if (pid < 0)
        error("Fork Error");
    // Child process but execution failed
    else if (pid == 0 && execvp(cmdLine->arguments[0], cmdLine->arguments) == -1)
        error("Execution Error");
    // Parent process and command is blocking
    else if (pid > 0 && cmdLine->blocking)
        waitpid(pid, NULL, 0);
}

void error(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

int signalProcess(char *pid, int signal)
{
    int PID = atoi(pid);
    if (kill(PID, signal) < 0)
        error("Waking Process Error");

    return 1;
}

int cd(char *path)
{
    if (chdir(path) < 0)
        error("Changing Directories Error");

    return 1;
}