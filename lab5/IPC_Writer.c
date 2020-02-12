/***********************************************************************
* Author:    Collin Larson & William Mckeever
* Class:     CIS 452, Lab 5
* Due:       Feb 13th, 2020
* Description: Writer Program for IPC that creates a shared memory
* segment, then repeatably accepts an arbitrary user input string 
* from the terminal and writes it into shared memory.
***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <string.h>

#define BUFF 500
#define FOO 4096

/** Helper Methods */
void sigHandlerShutdown(int);
void prompt();

/** Shared Struct in memory */
struct shMemSeg {
  int readerCount;
  int readCount;
  int wordCount;
  int complete;
  char buffer[BUFF];
};

int shmId;
key_t shMemKey;
struct shMemSeg *shMemMessages;

int main()
{
  /** SIGINIT is a signal from a keyboard */
  signal(SIGINT, sigHandlerShutdown);
  // /home/larsonco/CIS_452/labs/lab5/sample1.c // wasnt return the same key everytime...
  if ((shMemKey = ftok("/",34)) == (key_t) -1) {
    perror("IPC error: ftok\n");
    exit(1);
  }

  printf("ftok Key: %ld\n", (long)shMemKey);

  if ((shmId = shmget (shMemKey, FOO, IPC_CREAT|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) < 0) {
      perror ("i can't get no..\n");
      exit (1);
  }

  if ((shMemMessages = shmat(shmId, 0, 0)) == (void*) -1)  {
    perror("Cant attach\n");
    exit(1);
  }

  prompt();
  shMemMessages->complete  = 0;
  shMemMessages->readCount = 0;
  shMemMessages->wordCount = 1;

  printf("Data written in memory: %s\n",shMemMessages->buffer); 
  while (1) {
    if (shMemMessages->readerCount != 0) {
      if (shMemMessages->readCount == shMemMessages->readerCount) {
        printf("Read Count: %d. Reader Count: %d\n", shMemMessages->readCount, shMemMessages->readerCount);
        shMemMessages->wordCount = 0;
        prompt();
        shMemMessages->wordCount = 1;
        shMemMessages->readCount = 0;
      } else {
        printf("Read Count: %d. Reader Count: %d\n", shMemMessages->readCount, shMemMessages->readerCount);
      } // end of readCount == reader count
    } else {
      printf("Waiting for reader to join.\n");
    } // end of readerCount 
  }
  
  return 0;
} // end of main

void prompt() {
  printf("Enter a message: ");
  fgets(shMemMessages->buffer, BUFF, stdin);
}

void sigHandlerShutdown(int sigNum) {
  printf("\nDettaching.\n");
  shmdt(shMemMessages);
  shmctl(shmId, IPC_RMID, 0);
  exit(1);
} // end of ctrl^ handler
