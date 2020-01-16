/***********************************************************************
* A trivial shell was created that allows the users to run programs in
* series. The user can enter an arbitrary number of commands. The
* command "Execute" initiates execution of the sequence of commands.
* Timing data is also tracked and reported for each command as well as
* the time it took to run the program in its entirety.
* After all programs have completed, the command name, exit code, and
* elapsed time for each command is printed to the screen for the user.
*
*Author:
*           Andrew Weston,
*           Collin Larson,
*           Katie LaBell
*Class:     CIS 241, Project 4
*Due:       AUG 2nd, 2019
***********************************************************************/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <time.h>



/* Struct used user commands and data */
typedef struct input {
        char* command;
        char** av;
        int ac;
        pid_t pid;
        int status;
        double cpu_time_used;

} input;

/* Defining variables and functions */
extern int errno;

/* Function to greet the user*/
void hello();

/* Function to parse input into argv */
char** parse_args(char*, int*);

/* Execute user's commands in series */
void execute(struct input* commands, size_t size);

/* Print summary details */
void printSummary(input* commands, size_t size);

/* Free struct memory */
void freeAll(input* commands, size_t size);


/***********************************************************************
 * This function is the program driver.  It prompts the user to enter
 * a commands, executes the commands, and prints summary data.
 *
 * @param argc the number of arguments.
 * @param argv the vector of char* arguments.
 * @return is an integer representation of program exit status.
 **********************************************************************/
int main(int argc, char** argv) {
        int errnum;
        int counter      = 0;           //number of commands
        int exc          = 0;           //loop control variable

        //mallocing space for the first command
        struct input* in = (struct input*) malloc(sizeof(struct input));

        if(in == NULL) {
                perror("Memory allocation failed");
                exit(ENOMEM);
        }
        hello();

        //while accepting user input
        while(exc == 0) {
          in[counter].command = readline
                  ("Enter a command ('Execute' to run): ");

          //checking if user is finished entering commands
          if(strncmp(in[counter].command, "Execute", 7) == 0) {
            exc = 1;
          } else {
            //parse user input
            in[counter].av = parse_args(in[counter].command,
                            &in[counter].ac);

            //realloc space for the next command
            counter = counter + 1;
            in = (struct input*)
                    realloc(in, ((counter+1) * sizeof(struct input)));
            if(in == NULL) {
                perror("Memory allocation failed");
                exit(ENOMEM);
        }

          }

        }

        execute(in, counter);
        printSummary(in, counter);
        free(in);
        return 0;
}



/***********************************************************************
 * This function greets the user.
 **********************************************************************/
void hello() {
        char* username = getenv("USER");
        printf("Hello %s\n", username);
}


/***********************************************************************
 * This function splits the char* line into tokens using whitespace as
 * a delimiter.
 *
 * @param char* line represents a command line command.
 * @param argc is the int* to the number of command line arguments.
 * @return is the char* vector of split command like tokens.
 **********************************************************************/
char** parse_args(char* line, int* argc) {

        char** av = (char**)malloc(sizeof(char**));
        *argc = 0;

        char* delim = " \t\n";
        char* tok;

        char* cp_line;
        cp_line = (char*)malloc(strlen(line)*sizeof(char));
        strcpy(cp_line, line);
        tok = strtok(cp_line, delim);

        while(tok != 0) {
                av[*argc]=tok;
                *argc += 1;
                av = (char**)
                        realloc(av, ((*argc) + 1)  * sizeof(char**));
                tok = strtok(0, delim);
        }
        av[*argc] = NULL;

        return av;
}


/***********************************************************************
 * This function executes a series of user-entered commands.
 *
 * @param commands is an input* to the first element of an input array.
 * @param size is the number of input elements.
 **********************************************************************/
void execute(input* commands, size_t size) {
        for(int i =0; i < size; ++i) {
            pid_t c_pid, pid;
            commands[i].status = 0;
            int stat = 0;
            clock_t start, end;
            double cpu_time_used;

            start = clock();            // records processer time
            //start = time(NULL);       // records real time
            // We were not sure which time you wanted to record
            // so we just did processor instead of real time.
            c_pid = fork();
            if(c_pid == 0) {
               int exec_stat = 0;
               exec_stat = execvp(commands[i].av[0], commands[i].av);
              if(exec_stat == -1){
                exit(errno);
              }

            } else if (c_pid > 0) {
               commands[i].pid    = wait(&stat);
               commands[i].status = WEXITSTATUS(stat);
               end                = clock();
               //end                = time(NULL);
               commands[i].cpu_time_used = //end - start;
                       ((double)(end -start))/CLOCKS_PER_SEC;
            } else {
              perror("fork failed");
              _exit(2);
            }
        }
}


/***********************************************************************
 * This function prints summary data for a series of user-entered
 * commands.
 *
 * @param commands is an input* to the first element of an input array.
 * @param size is the number of input elements.
 **********************************************************************/
void printSummary(input* commands, size_t size) {
        double sum_time = 0.0;
        for(int i =0; i < size; ++i){
                printf("\nCommand: %s\n", commands[i].command);
                printf("PID: %d\n", commands[i].pid);
                printf("Exit code: %d\nElapsed Time(sec): %f\n\n",
                                commands[i].status,
                                commands[i].cpu_time_used);

                sum_time += commands[i].cpu_time_used;
        }
        if(size >0){
                printf("Total Program Run Time: %f seconds.\n",
                                sum_time);
        }
}
