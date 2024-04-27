#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

//TODO: Read the file location and thread number from the command line arguments
#define FILE_LOCATION "/home/furkan/Desktop/CMPE382/HW2/CMPE382-HW2-CriticalSectionSynchronization/myDir/file1.txt"
#define SIMULTANEOUS_THREAD_COUNT 1

// Function prototypes
int isPrime(int number);
void* countPrimes(void* arg);

int main(int argc, char *argv[]) {
    pthread_t thread1;
    
    //TODO: Distribute the work between threads
    //TODO: Make number of active threads limited to threadNumber
    //TODO: Use semaphores (mutexes) to limit the number of simultaneously active threads
    //TODO: Create threads whenever an active thread is done
    //TODO: Make the program generic to work with any number of files
    //TODO: Observe time data

    pthread_create(&thread1, NULL, countPrimes, NULL);
    pthread_join(thread1, NULL);
    return 0;
}

void* countPrimes(void* arg) {
    FILE *fp = NULL;
    char* line = NULL;
    size_t len = 0;
    int count = 0;
    int readLineCount = 0;

    fp = fopen(FILE_LOCATION, "r");

    if (fp == NULL) {
        perror("Error opening file");
        return NULL;
    }

    while (getline(&line, &len, fp) != -1) {
        int number = atoi(line);
        if (isPrime(number))
            count++;
        readLineCount++;
    }

    printf("Thread 1 has found %d prime numbers\n", count);

    // Free resources and close the file
    if (line) {
        free(line);
    }
    fclose(fp);
}

int isPrime(int number)
{
    if (number <= 1)
        return 0;
    for (int i = 2; i < number; i++)
        if (number % i == 0)
            return 0;
    return 1;
}