#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    char inbuf[100];
    int pipeFileDescriptor[2], pid, nbytes;

    if (pipe(pipeFileDescriptor) < 0)
        exit(1);

    if ((pid = fork()) > 0)
    {
        printf("parent with pid: %d, Starting to read\n", getpid());

        wait(NULL);
        if ((nbytes = read(pipeFileDescriptor[0], inbuf, 100)) > 0)
            printf("%s\nparent with pid: %d, Finished reading\n", inbuf, getpid());
        if (nbytes < 0)
            exit(1);
    }
    else
    {
        printf("child with pid: %d, Starting to write\n", getpid());
        
        if ((nbytes = write(pipeFileDescriptor[1], "hello", 100)) > 0)
            printf("child with pid: %d, Finished writing\n", getpid());
        if (nbytes < 0)
            exit(1);
    }
    return 0;
}
