#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

static int runningChild;

void childProcess(void);
void sigHandler1(int);
void sigHandler2(int);
void sigHandlerShutdown(int);

int main()
{
    pid_t pid;
    int status;

    signal(SIGINT, sigHandlerShutdown);

    if ((pid = fork()) < 0)             //Child Process
    {
        perror("fork failure");
        exit(1);
    } 
    else if (pid == 0)                    //Parent Process
    {              
        childProcess();
    } 
    else 
    {
        wait(&status);                    //Waiting for child to finish
    }

    return 0;
}

void childProcess(void)
{
    int randTime;
    int randProcess;

    runningChild = 1;

    signal(SIGUSR1, sigHandler1);
    signal(SIGUSR2, sigHandler2);

    while(runningChild)
    {
        randTime = (rand() % 5) + 1;
        randProcess = (rand() % 2) + 1;

        printf("waiting...\t");
        sleep(randTime);

        if(randProcess == 1)
        {
            raise(SIGUSR1);
        }
        else
        {
            raise(SIGUSR2);
        }
    }
}

void sigHandler1(int sigNum)
{
    printf(" received a SIGUSR1 signal.\n");

} 

void sigHandler2(int sigNum)
{
    printf(" received a SIGUSR2 signal.\n");
} 

void sigHandlerShutdown(int sigNum)
{
    printf(" received. That's it, I'm shutting you down...\n");
    runningChild = 0;
} 