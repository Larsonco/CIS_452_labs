/**
* Author: Johnathon Frocillo and Katie LaBell
* CIS 452-10: Operating Systems Concepts
* February 6, 2020
* Lab 4 Coding Part

* This program simulates the thread execution manifested by a multi-threaded fileserver process.
* The user enters a "file" to be "retrieved" by the simulated server.
* A child thread is fired off to retrieve the file, and the user can immediately request another,
*       while waiting for the first to return.
 */

#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

void* retrieveFile(void* arg);
void sigHandler (int);

//Shared data / counter variables
int numOfFilesRequested;
double averageAccessTime;

//the file the user wants to retrieve
char userFile[20];

//make sure data is not "clobbered" or overwritten
pthread_mutex_t lock;

//random probability for sleeping 1 or 7-10 secs
int randomProb;
//random 7-10 second sleep
int random7_10;

int main() {
    //create the mutex lock
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }
    
    //initialize number of files requested
    numOfFilesRequested = 0;
    
    //worker thread to handle the file retrieval
    pthread_t workerThread;
    
    //captures the retun of creating threads
    int status;

    //holds the return value
    void *result;
    
    //set up the signal to react to ctrl-c
    signal (SIGINT, sigHandler);
    
    while(1) {
        printf("\nEnter a file to retrieve: ");
        
        scanf("%s", userFile);
        
        fflush(stdin);
        
        //create the threads to handle the file
        if ((status = pthread_create (&workerThread, NULL,  retrieveFile, &userFile)) != 0) {
            fprintf (stderr, "thread create error %d: %s\n", status, strerror(status));
            exit (1);
        }
        
    }

    //join the threads
    if ((status = pthread_join (workerThread, &result)) != 0) {
        fprintf (stderr, "join error %d: %s\n", status, strerror(status));
        exit (1);
    }
    

    //destroy the mutex
    pthread_mutex_destroy(&lock);
    
    return 0;
}

void* retrieveFile(void* arg) {
    //lock the mutex
     pthread_mutex_lock(&lock);
    
    //cast the void pointer to the desired data type
    char *file = (char *)arg;
    
    //truely randomize
    srand(time(NULL));
    //random number 1-10
    randomProb = rand() % 10 + 1;
    
    //80% to sleep for 1 second, 20% chance to randomly sleep 7-10 secs
    if(randomProb <= 8) {
        sleep(1);
        averageAccessTime ++;
    }
    else {
        //truely randomize
        srand(time(NULL));
        random7_10 = rand() % 4 + 7;
        sleep(random7_10);
        averageAccessTime += (double)random7_10;
    }
    
    //obtain the simulated filename from the dispatcher
    printf("\nFile %s Retrieved.\n\n", file);
    numOfFilesRequested ++;
    
    //unlock the mutex
    pthread_mutex_unlock(&lock);
    return arg;
}

void sigHandler(int sigNum) {
    printf("\nCtrl-C received. Shutting you down now... \n");
    
    //print out statistics: total number of files requested,
    // and average access time
    printf("Number of files requested: %d\n", numOfFilesRequested);
    averageAccessTime = averageAccessTime / (double) numOfFilesRequested;
    printf("Average access time: %lf seconds\n\n", averageAccessTime);
    
    //end the program and all threads
    exit(0);
}
