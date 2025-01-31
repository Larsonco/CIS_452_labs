#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define SIZE 16

int main (int argc, char* argv[])
{
   int status;
   long int i, loop, temp, *shmPtr;
   int shmId;
   pid_t pid;

   if (argc < 2) {
      printf("Usage: user must supply a loop variable!\n");
      exit(1);
   }

   loop = atoi(argv[1]);  // get value of loop variable (from command-line argument)
   printf("Loop var: %li\n", loop);

   if ((shmId = shmget (IPC_PRIVATE, SIZE, IPC_CREAT|S_IRUSR|S_IWUSR)) < 0) {
      perror ("i can't get no..\n");
      exit (1);
   }
   if ((shmPtr = shmat (shmId, 0, 0)) == (void*) -1) {
      perror ("can't attach\n");
      exit (1);
   }

   shmPtr[0] = 0;
   shmPtr[1] = 1;

   if (!(pid = fork())) {   // I am the child (!(0))
      for(i = 0; i < loop; i++) {
        /** swap the contents of shmPtr[0] and shmPtr[1] */
        temp      = shmPtr[0];
        shmPtr[0] = shmPtr[1];
        shmPtr[1] = temp;
      }
      if (shmdt (shmPtr) < 0) {
         perror ("just can't let go\n");
         exit (1);
      }
      exit(0);
   }
   else // I am the parent
      for (i=0; i<loop; i++) {
        /** swap the contents of shmPtr[1] and shmPtr[0] */
        temp      = shmPtr[1];
        shmPtr[1] = shmPtr[0];
        shmPtr[0] = temp;
      }

   wait (&status);
   printf ("values: %li\t%li\n", shmPtr[0], shmPtr[1]);

   if (shmdt (shmPtr) < 0) {
      perror ("just can't let go\n");
      exit (1);
   }
   if (shmctl (shmId, IPC_RMID, 0) < 0) {
      perror ("can't deallocate\n");
      exit(1);
   }

   return 0;
}