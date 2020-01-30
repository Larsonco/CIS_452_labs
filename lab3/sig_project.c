/***********************************************************************
* Author:    Collin Larson & William Mckeever
* Class:     CIS 452, Lab 3
* Due:       Jan 30th, 2020
* Description: Lab Programming Assignment (Communicating Processes)
***********************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

static int runningChild;

/** Helper methods */
void childProcess(void);
void sigHandler1(int);
void sigHandler2(int);
void sigHandlerShutdown(int);

/** Main method ran for sig_project.c */
int main()
{
    pid_t pid;
    int status;

    /** SIGINIT is a signal from a keyboard */
    signal(SIGINT, sigHandlerShutdown);

    if ((pid = fork()) < 0) {
        perror("fork failure");
        exit(1);
    } else if (pid == 0) {              
        childProcess();
    } else {
        wait(&status);  
    }

    return 0;
}   // end of main

void childProcess(void)
{
    int randTime;
    int randProcess;

    runningChild = 1;

    /** SIGUSER1 & 2 ared user defined signal 1 & 2*/
    signal(SIGUSR1, sigHandler1);
    signal(SIGUSR2, sigHandler2);

    while(runningChild)
    {
        randTime = (rand() % 5) + 1;
        randProcess = (rand() % 2) + 1;

        printf("waiting...\t");
        sleep(randTime);

        if (randProcess == 1) {
            raise(SIGUSR1);
        } else {
            raise(SIGUSR2);
        }   // end of if
    }   // end of while
}   // end of childProcess

void sigHandler1(int sigNum) {
    if(runningChild)
        printf(" received a SIGUSR1 signal.\n");
}   // end of sigHandler1

void sigHandler2(int sigNum) {
    if(runningChild)
        printf(" received a SIGUSR2 signal.\n");
}   // end of sigHandler2

void sigHandlerShutdown(int sigNum) {
    printf(" received. That's it, I'm shutting you down...\n");
    runningChild = 0;
}   // end of parent sigHandler
