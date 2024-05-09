#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

#define FILE_NAME_TEMPLATE "file%d.txt"

// Function prototypes
int isPrime(int number);
void* countPrimes(void* arg);
void generate_file_path(char *buffer, size_t buffer_size, const char *directory, int file_number);

// Struct to pass arguments to the thread
typedef struct _thstruct {
    int file_number;
    char* directory_arg;
} THREAD_ARG;

// Semaphores as global variables
sem_t simultaneous_thread_limit_sem;
sem_t mutex;

int main(int argc, char *argv[]) {
    int simultaneous_thread_count = atoi(argv[2]); // argv[2] is the number of threads

    printf("Simultaneous thread count: %d\n", simultaneous_thread_count);

    // Counting how many files there are in the directory
    int current_file_number = 1;
    char file_path[256];  // Buffer to hold the file path
    FILE *file;

    do {
        // argv[1] is the directory location (for example, "myDir/")
        generate_file_path(file_path, sizeof(file_path), argv[1], current_file_number); // Generate the file path

        file = fopen(file_path, "r");

        if (file) {
            fclose(file);  // Close the file to avoid memory leaks
            current_file_number++; // Move to the next file
        }
    } while (file);  // Continue until fopen returns NULL (i.e. the file does not exist)

    int file_count = current_file_number - 1; // Subtract 1 because the last file does not exist

    printf("There are %d files in the directory\n", file_count);
    // End of counting files


    sem_init(&simultaneous_thread_limit_sem, 0, simultaneous_thread_count); // Initialize the semaphore with the number of threads
    sem_init(&mutex, 0, 1); // Not used yet ???

    pthread_t threads[file_count];
    THREAD_ARG thread_args[file_count];

    for(int i = 0; i < file_count; i++) { // Sending the file number to the thread as an argument
        thread_args[i].file_number = i + 1;
        thread_args[i].directory_arg = argv[1];
    }

    for(int i = 0; i < file_count; i++) {
        pthread_create(&threads[i], NULL, countPrimes, &thread_args[i]);
    }

    //TODO: Create threads whenever an active thread is done ???
    //TODO: Observe the time data from terminal

    for (int i = 0; i < file_count; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

void generate_file_path(char *buffer, size_t buffer_size, const char *directory, int file_number) {
    // All files are named file1.txt, file2.txt, file3.txt, etc. Therefore, I can use a template to generate the file name
    // argv[1] serves as the directory location
    snprintf(buffer, buffer_size, "%s" FILE_NAME_TEMPLATE, directory, file_number);
}

void* countPrimes(void* arg) {
    // Limiting the # of threads that can enter the critical section (countPrimes method in this case) is simultaneous_thread_count
    sem_wait(&simultaneous_thread_limit_sem);

    THREAD_ARG* thread_arg = (THREAD_ARG*)arg; // Storing the argument in a local variable

    FILE *fp = NULL;
    char* line = NULL;
    size_t len = 0;
    int count = 0;
    int readLineCount = 0;

    char file_path[256];
    generate_file_path(file_path, sizeof(file_path), thread_arg->directory_arg, thread_arg->file_number);

    printf("This thread is reading file %s\n", file_path);

    fp = fopen(file_path, "r");

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

    printf("This thread has found %d prime numbers\n\n", count);

    // Free resources and close the file
    if (line) {
        free(line);
    }
    fclose(fp);

    sem_post(&simultaneous_thread_limit_sem);
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