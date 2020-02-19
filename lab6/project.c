#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>

#define SIZE 16

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

//semwait
struct sembuf p = {0, -1, SEM_UNDO};

//semsignal
struct sembuf v = {0, 1, SEM_UNDO};

int main (int argc, char* argv[])
{
   int status;
   long int i, loop, temp, *shmPtr;
   int shmId;
   int semId;
   pid_t pid;

   if (argc < 2) {
      printf("Usage: user must supply a loop variable!\n");
      exit(1);
   }

   loop = atoi(argv[1]);  // get value of loop variable (from command-line argument)
   printf("Loop var: %li\n", loop);

    // 2nd argument is number of semaphores
    // 3rd argument is the mode (IPC_CREAT creates the semaphore set if needed)
    if ((semId = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT)) < 0) {
      perror ("i can't get a sem\n");
      exit (1);
    }


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
        
        if(semop(semId, &p, 1) < 0)
        {
            perror("semop p"); exit(1);
        }
        
        for(i = 0; i < loop; i++) {
            /** swap the contents of shmPtr[0] and shmPtr[1] */
            temp      = shmPtr[0];
            shmPtr[0] = shmPtr[1];
            shmPtr[1] = temp;
        }

        if(semop(semId, &v, 1) < 0)
        {
            perror("semop p"); exit(1);
        }

        if (shmdt (shmPtr) < 0) {
            perror ("just can't let go\n");
            exit (1);
        }
        exit(0);
   }
   else // I am the parent
   {
        union semun u;
        u.val = 1;
        if (semctl(semId, 0, SETVAL, u) < 0) { // SETVAL is a macro to specify that you're setting the value of the semaphore to that specified by the union u
            /* error handling code */
        }

        if(semop(semId, &p, 1) < 0)
        {
            perror("semop p"); exit(1);
        }

        for (i=0; i<loop; i++) {
            /** swap the contents of shmPtr[1] and shmPtr[0] */
            temp      = shmPtr[1];
            shmPtr[1] = shmPtr[0];
            shmPtr[0] = temp;
        }

        if(semop(semId, &v, 1) < 0)
        {
            perror("semop p"); exit(1);
        }
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
