#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#define SEC_TO_USEC_FACTOR 1000000
#define N_MAX_AVAILABLE_RESOURCES 5
#define N_MAX_THREADS 100
#define N_WORKS_TO_BE_DONE 100
int n_works_done = 0;
int n_currently_used_resources = 0;
void consume_resource_and_do_work(int thread_id);
/*
    TO-DO: Global variables goes here
*/
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;
void* thread_routine(void* arg) {    
    // Missing logic goes here if any
    pthread_mutex_lock(&thread_mutex);
    while (n_currently_used_resources == N_MAX_AVAILABLE_RESOURCES)
    {
        pthread_cond_wait(&cond, &thread_mutex);
    }
    pthread_mutex_unlock(&thread_mutex);
    /* TO-DO: Change ????? with appropriate expression below */
    consume_resource_and_do_work((int)arg);
    /* TO-DO: Change ????? with appropriate expression above */
    
    // Missing logic goes here if any
    //pthread_mutex_lock(&thread_mutex);
    pthread_cond_signal(&cond);
    //pthread_mutex_unlock(&thread_mutex);
}
void manage_threads() {
    // You can initialize required things here if any
    /* TO-DO: Only change ?????s with appropriate expressions below */
    pthread_t tids[100];
    for(int i = 0; i < 100; i++) {
        pthread_create(&tids[i], NULL, thread_routine, (void*)i);
    }
    /* TO-DO: Only change ????? with appropriate expressions above */
    // Missing logic goes here if any
    for(int i = 0; i < 100; i++) {
        pthread_join(tids[i], NULL);
    }
    //pthread_exit(NULL);
}
/* 
    Don't change anything below this section!
    Don't change anything below this section!
    Don't change anything below this section!
*/
pthread_mutex_t resource_mutex;
int check_all_works_done();
int main() {
    pthread_mutex_init(&resource_mutex, NULL);
    
    struct timeval check_point;
    gettimeofday(&check_point, NULL);
    long beginning_time_in_usec = (long) check_point.tv_sec * SEC_TO_USEC_FACTOR +  (long) check_point.tv_usec;
    
    manage_threads();
    
    gettimeofday(&check_point, NULL);
    long end_time_in_usec = (long) check_point.tv_sec * SEC_TO_USEC_FACTOR +  (long) check_point.tv_usec;
    
    pthread_mutex_destroy(&resource_mutex);
    
    double elapsed_time_in_sec = (end_time_in_usec - beginning_time_in_usec) / (double) SEC_TO_USEC_FACTOR;
    printf("[INFO] Elapsed time in seconds: %f\n", elapsed_time_in_sec);
    return check_all_works_done();   
}
void consume_resource_and_do_work(int thread_id) {
    pthread_mutex_lock(&resource_mutex);
    if(n_currently_used_resources == N_MAX_AVAILABLE_RESOURCES) {
        fprintf(stderr, "[ERROR] Only %d resoures can be used at max\n", N_MAX_AVAILABLE_RESOURCES);
        exit(EXIT_FAILURE);
    }
    n_currently_used_resources++;
    pthread_mutex_unlock(&resource_mutex);
    
    printf("Thread %d begins to work\n", thread_id);
    sleep(5);
    printf("Thread %d has finished its works\n", thread_id);
    pthread_mutex_lock(&resource_mutex);
    n_currently_used_resources--;
    n_works_done++;
    pthread_mutex_unlock(&resource_mutex);
}
int check_all_works_done() {
    if(n_works_done == N_WORKS_TO_BE_DONE) {
        fprintf(stdout, "[INFO] All works has been done, the program has finished correctly :)\n");
        return EXIT_SUCCESS;
    }
    else {
        fprintf(stdout, "[ERROR] There are still some works that should have been completed! :(\n");
        return EXIT_FAILURE;
    }
}