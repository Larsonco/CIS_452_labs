#include <pthread.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <cstdlib>
#include <string>
#include <memory>
#include <vector>

void* worker_threads(void* arg);

int main()
{
    while(1)
    {
        //std::cout << "Please Enter A String: " << std::endl;
        std::string userInput;
        std::getline(std::cin, userInput); 
        if(userInput.find("quit") != std::string::npos)
        {
            std::cout << "Exiting Program" << std::endl;
            exit(1);
        }

        std::string *userRequest = new std::string(userInput);

        int status;
        pthread_t threadID;
        if ((status = pthread_create(&threadID, NULL, worker_threads, userRequest)) != 0) {
            std::cerr << "thread create error: " << strerror(status) << std::endl;
            exit(1);
        }
    }
    return 0;
}

void* worker_threads(void* arg)
{
    std::string* userRequest = (std::string*) arg;
    int random;
    random = (rand() % 10) + 1;
    if(random < 9) {
        sleep(1);
    } else {
        int randomTime = (rand() % 4) + 8;
        sleep(randomTime);
    }
    

    //std::cout << "Thread Output: " << std::endl;
    std::cout << *userRequest << std::endl;
    return NULL;
}