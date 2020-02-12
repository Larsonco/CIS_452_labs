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

/** Shared Struct in memory */
struct shMemSeg {
  int readerCount;
  int readCount;
  int wordCount;
  int complete;
  char buffer[BUFF];
};

int shmId;
int myPosition;
key_t shMemKey;
struct shMemSeg *shMemMessages;

int main() {

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

  if (shMemMessages->readerCount < 0 || shMemMessages->readerCount > 10) { // check if readerCount was initialized.
    shMemMessages->readerCount = 0;
  }

  myPosition = shMemMessages->readerCount + 1;
  shMemMessages->readerCount = shMemMessages->readerCount + 1;

  while(1) {
    if (shMemMessages->wordCount > 0) {                             // making sure word is ready to be read
      if ((shMemMessages->readCount+1) == myPosition) {             // my turn to read
        printf("Messege from writer: %s\n", shMemMessages->buffer);
        shMemMessages->readCount = (shMemMessages->readCount + 1);
      }
    }
  } // end of while

  return 0;

} // end of main

void sigHandlerShutdown(int sigNum) {
  printf("\nDettaching.\n");
  shmdt(shMemMessages);
  shmctl(shmId, IPC_RMID, 0);
  exit(1);
} // end of ctrl^ handler
