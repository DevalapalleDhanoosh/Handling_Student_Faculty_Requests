#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// Maximum number of requests the system can handle
const int MAX_REQUESTS = 100;

// Structure to represent a request
typedef struct Request {
    int id;
    int type; // 0 for student, 1 for faculty
} Request;

// Global variables
Request requestQueue[MAX_REQUESTS]; // Use MAX_REQUESTS to define the size
int numRequests = 0;
sem_t mutex, studentSem, facultySem;
int totalTime = 0;
int totalRequests = 0; // New variable to track total requests processed

// Function to process a request
void processRequest(Request *request) {
    // Simulate processing time (in seconds)
    sleep(5); // You can adjust this time as needed
    
    // Update total time
    totalTime += 5; // Again, adjust as needed

    printf("Processed request %d (Type: %s)\n", request->id, request->type == 0 ? "Student" : "Faculty");
}

// Function to handle requests
void *handleRequest(void *arg) {
    while (1) {
        sem_wait(&mutex);
        
        if (numRequests == 0) {
            sem_post(&mutex);
            break;
        }

        Request request = requestQueue[--numRequests];
        sem_post(&mutex);

        processRequest(&request);

        // Increment totalRequests
        totalRequests++;
    }

    pthread_exit(NULL);
}

// Function to add a request to the queue
void addRequest(Request *request) {
    sem_wait(request->type == 0 ? &studentSem : &facultySem);
    
    sem_wait(&mutex);
    requestQueue[numRequests++] = *request;
    sem_post(&mutex);
}

int main() {
    sem_init(&mutex, 0, 1);
    sem_init(&studentSem, 0, MAX_REQUESTS);
    sem_init(&facultySem, 0, MAX_REQUESTS);

    pthread_t tid;

    // Create threads to handle requests
    pthread_create(&tid, NULL, handleRequest, NULL);

    // Simulate requests being added (you can replace this with actual input logic)
    Request req1 = {1, 0}; // Student request
    Request req2 = {2, 1}; // Faculty request
    Request req3 = {3, 0}; // Student request

    addRequest(&req1);
    addRequest(&req2);
    addRequest(&req3);

    // Wait for thread to finish
    pthread_join(tid, NULL);

    // Print summary
    if (totalRequests > 0) {
        printf("Total time spent: %d seconds\n", totalTime);
        printf("Average query time: %.2f seconds\n", (float)totalTime / totalRequests);
    } else {
        printf("No requests processed.\n");
    }

    return 0;
}

