/***********************************************************************
* Author:    Collin Larson & William Mckeever
* Class:     CIS 452, Lab 4
* Due:       Feb 6th, 2020
* Description: This file simulates thread execution manifested by
*   a multi-threaded fileserver process;
***********************************************************************/
#include <pthread.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <cstdlib>
#include <string>
#include <memory>
#include <vector>

/** Helper Methods */
void *worker_threads(void *arg);
void sigHandlerShutdown(int);

/** Number of threads spawned */
int requestCount = 0;

/** Main method ran for lab4_project.cpp */
int main()
{
  /** SIGINIT is a signal from a keyboard */
  signal(SIGINT, sigHandlerShutdown);

  while (1)
  {
    //std::cout << "Please Enter A String: " << std::endl;
    std::string userInput;
    std::cout << "Enter Filename: ";
    std::getline(std::cin, userInput);

    if (userInput.find("quit") != std::string::npos)
    {
      std::cout << std::endl
                << "File requests received : " << requestCount << std::endl;
      std::cout << "Exiting Program" << std::endl;
      exit(1);
    }

    requestCount++;

    /** String pointer to user input */
    std::string *userRequest = new std::string(userInput);

    /** Thread Status */
    int status;

    /** Thread Id */
    pthread_t threadID;

    /** Creation of thread */
    if ((status = pthread_create(&threadID, NULL, worker_threads, userRequest)) != 0)
    {
      std::cerr << std::endl
                << "thread create error: " << strerror(status) << std::endl;
      exit(1);
    }
  }

  return 0;
}

/********************************************
 * @description: Function to be executed for 
 * the thread to be created
 * @params: void* arg - The user request
 ********************************************/
void *worker_threads(void *arg)
{
  /** Mapping void* arg to a string* */
  std::string *userRequest = (std::string *)arg;

  int random;
  random = (rand() % 10) + 1;

  if (random < 9)
  {
    sleep(1);
  }
  else
  {
    int randomTime = (rand() % 4) + 8;
    sleep(randomTime);
  }

  std::cout << std::endl
            << "Diagnostic message: ";
  std::cout << *userRequest << std::endl;
  delete userRequest;
  return NULL;
}

void sigHandlerShutdown(int sigNum)
{
  std::cout << std::endl
            << "File requests received : " << requestCount << std::endl;
  std::cout << "Exiting Program" << std::endl;
  exit(1);
}
