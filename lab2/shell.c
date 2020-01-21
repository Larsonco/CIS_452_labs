#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#define MAX 15

/** Helper methods */
void prompt();
void parse_args();
void execute();

char inputCmd[MAX];
char* inputArgs[MAX];
struct timeval oldTim;
int cmdCount = 0;

int main(int argc, char **argv) {
  while (1) {
    prompt();

    if (strncmp(inputCmd, "quit", 4) == 0)
      break;

    parse_args();
    execute();
    cmdCount += 1;
  } // end of while

  return 0;
} // end of main

void prompt() {
  printf("Enter a command to execute (15 char max, quit to quit): ");
  fgets(inputCmd, MAX, stdin);
}

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

void execute() {
  pid_t child, pid;
  int status;

  struct rusage usage;                // refer to man getrusage(2)
  struct timeval tim;                 // timeval struct
  long ivcsw;                         // involuntary context switches

  if ((pid = fork()) < 0) {
    perror("fork failure");
    exit(1);
  } else if (pid == 0) {
    execvp(inputArgs[0], inputArgs);
    exit(0);
  } else {
    wait(&status);
    getrusage(RUSAGE_SELF, &usage);
    ivcsw = usage.ru_nivcsw;
    tim = usage.ru_utime;

    //if (cmdCount == 0) {
      printf("Time elasped at: %ld.%lds\n", tim.tv_sec, tim.tv_usec);
    //} 
    // else {
    //   printf("Time elasped at: %ld.%lds\n", 
    //     (tim.tv_sec-oldTim.tv_sec), (tim.tv_usec-oldTim.tv_usec));
    // }

    oldTim = tim;    
    printf("Total involuntary context switches: %ld\n", ivcsw);
  }

}
