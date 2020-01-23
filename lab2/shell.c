/***********************************************************************
* Author:    Collin Larson & William Mckeever
* Class:     CIS 241, Project 4
* Due:       Jan 23rd, 2020
* Description: 
***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>             //Added to get rid of warnings.

#define MAX 15

/** Helper methods */
void prompt();
void parse_args();
void execute();

/** Initial command inputted by user */
char inputCmd[MAX];

/** Parsed command and args */
char* inputArgs[MAX];

/** Previous user cpu time */
struct timeval oldTim;

/** Command count */
int cmdCount = 0;

/** Main method ran for shell.c */
int main(int argc, char **argv) {
  while (1) {
    prompt();                                 // display prompt to user

    if (strncmp(inputCmd, "quit", 4) == 0)    // check if user wants to quit
      break;                                  // break out of while loop

    parse_args();                             // parse arguments from user
    execute();                                // execute cmd given by user
    cmdCount += 1;                            // increment # of commands ran
  } // end of while

  return 0;                                   // end program
} // end of main

/********************************************
 * void function that displays a promt &
 * gets user input
 *******************************************/
void prompt() {
  printf("Enter a command to execute (15 char max, quit to quit): ");
  fgets(inputCmd, MAX, stdin);
}

/********************************************
 * void function that parses the user input
 *******************************************/
void parse_args() {

  char* delim = " \t\n";
  char* tok;
  char cp_line[MAX];

  strcpy(cp_line, inputCmd);
  tok = strtok(cp_line, delim);

  int counter = 0;

  while(tok != NULL) {
    inputArgs[counter] = tok;
    counter += 1;
    tok = strtok(NULL, delim);
  } // end of while

} // end of parse_args;

/********************************************
 * void function that executs the user input
 * and display information about that
 * command's run time & inv context switches
 *******************************************/
void execute() {
  pid_t pid;
  int status;

  struct rusage usage;                // refer to man getrusage(2). Holds the current child proccess usage time.
  struct timeval tim;                 // timeval struct
  long ivcsw;                         // involuntary context switches

  if ((pid = fork()) < 0) {
    perror("fork failure");
    exit(1);
  } else if (pid == 0) {
    execvp(inputArgs[0], inputArgs);  // have child turn into the command to be executed
    exit(0);                          // returning success, not checking for erros
  } else {
    wait(&status);                    // waiting for child to finish
    //getrusage(RUSAGE_SELF, &usage);
    getrusage(RUSAGE_CHILDREN, &usage);
    ivcsw = usage.ru_nivcsw;
    tim = usage.ru_utime;

    if (cmdCount == 0) {
      printf("Time elasped at: %ld.%lds\n", tim.tv_sec, tim.tv_usec);
    } else {
       printf("Time elasped at: %ld.%lds\n", (tim.tv_sec-oldTim.tv_sec), (tim.tv_usec-oldTim.tv_usec));
    }

    oldTim = tim;    
    printf("Total involuntary context switches: %ld\n", ivcsw);
  }

}