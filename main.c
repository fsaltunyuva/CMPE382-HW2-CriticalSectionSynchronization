#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define FILE_LOCATION "myDir/file1.txt"
//#define DIRECTORY_LOCATION "myDir/"
#define FILE_NAME_TEMPLATE "file%d.txt"

// Function prototypes
int isPrime(int number);
void* countPrimes(void* arg);
void generate_file_path(char *buffer, size_t buffer_size, const char *directory, int file_number);

int main(int argc, char *argv[]) {
    pthread_t thread1;
    int simultaneous_thread_count = atoi(argv[2]); // argv[2] is the number of threads

    printf("Simultaneous thread count: %d\n", simultaneous_thread_count);

    // Counting how many files there are in the directory
    int current_file_number = 1;
    char file_path[256];  // Buffer to hold the file path

    FILE *file;

    do {
        generate_file_path(file_path, sizeof(file_path), argv[1], current_file_number); // Generate the file path

        file = fopen(file_path, "r");

        if (file) {
            fclose(file);  // Close the file to avoid memory leaks
            current_file_number++; // Move to the next file
        }
    } while (file);  // Continue until fopen returns NULL (i.e. the file does not exist)

    int file_count = current_file_number - 1; // Subtract 1 because the last file does not exist

    printf("There are %d files in the directory\n", file_count);
    
    //TODO: Distribute the work between threads
    //TODO: Make simultaneous_thread_count of active threads limited to threadNumber
    //TODO: Use semaphores (mutexes) to limit the simultaneous_thread_count of simultaneously active threads
    //TODO: Create threads whenever an active thread is done
    //TODO: Make the program generic to work with any simultaneous_thread_count of files
    //TODO: Observe time data

    pthread_create(&thread1, NULL, countPrimes, NULL);
    pthread_join(thread1, NULL);
    return 0;
}

void generate_file_path(char *buffer, size_t buffer_size, const char *directory, int file_number) {
    // All files are named file1.txt, file2.txt, file3.txt, etc. Therefore, I can use a template to generate the file name
    // argv[1] serves as the directory location
    snprintf(buffer, buffer_size, "%s" FILE_NAME_TEMPLATE, directory, file_number);
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