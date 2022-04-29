#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define SEC_TO_USEC_FACTOR 1000000

/* Struct for list nodes */
typedef struct list_node_s
{
    int data;
    struct list_node_s *next;
}list_node;

/* Struct for task nodes */
typedef struct task_node_s
{
    int task_num;
    int task_type; // insert:0, delete:1, search:2
    int value;
    struct task_node_s *next;
}task_node;
/* List operations */
int Insert(int value);
int Delete(int value);
int Search(int value);
/* Task queue functions */
void Task_queue(int n);                                                          // generate random tasks for the task queue
void Task_enqueue(int task_num, int task_type, int value);                       // insert a new task into task queue
int Task_dequeue(long my_rank, int *task_num_p, int *task_type_p, int *value_p); // take a task from task queue
void* work(void* my_rank);
void print_list();

// list node's first node
list_node* head = NULL;
// task node's first and last node
task_node* first_task = NULL;
task_node* last_task = NULL;

// global variable to set when all
// tasks are generated
int all_tasks_generated = 0;

// mutex and conditions for threads
pthread_mutex_t thread_mutex;
pthread_cond_t thread_cond;

int main(int argc, char* argv[])
{
    srand ( time(NULL) );

    // take first argument and set to
    // thread number
    char *thread_num = argv[1];
    int thread_number = atoi(thread_num);

    // take second argument and set to
    // task number
    char *task_num = argv[2];
    int task_number = atoi(task_num);

    // initialization of mutex and condition
    pthread_mutex_init(&thread_mutex, NULL);
    pthread_cond_init(&thread_cond, NULL);

    // time calculation
    // beginning time before the functions
    // I copied this code block from lab exercises code
    // shared by Emre teacher :)
    struct timeval check_point;
    gettimeofday(&check_point, NULL);
    long beginning_time_in_usec = (long) check_point.tv_sec * SEC_TO_USEC_FACTOR +  (long) check_point.tv_usec;

    // array for threads
    pthread_t tids[thread_number];
    for(int i = 0; i < thread_number; i++) {
        // create thread and pass them the work function
        // and i parameter
        pthread_create(&tids[i], NULL, work, (void*)(long) i);
    }
    
    // generate Task Queue
    Task_queue(task_number);

    // wait for the threads to terminate
    for(int i = 0; i < thread_number; i++) {
        pthread_join(tids[i], NULL);
    }

    // print final list
    print_list();

    // again time calculation
    // end time after the functions
    gettimeofday(&check_point, NULL);
    long end_time_in_usec = (long) check_point.tv_sec * SEC_TO_USEC_FACTOR +  (long) check_point.tv_usec;
        
    // destroy mutex and condition
    pthread_mutex_destroy(&thread_mutex);
    pthread_cond_destroy(&thread_cond);

    // calculation of elapsed time and printing it
    double elapsed_time_in_sec = (end_time_in_usec - beginning_time_in_usec) / (double) SEC_TO_USEC_FACTOR;
    printf("[INFO] Elapsed time in seconds: %f\n", elapsed_time_in_sec);  

    return 0;
}

int Insert(int value)
{
    // first point list's head with current variable
    list_node* current = head;
    // and store previous node's data
    list_node* previous = NULL;
    // iteration in list node to find where we will insert
    // the value (ascending order)
    while (current != NULL && current->data < value)
    {
        // set previous to current
        previous = current;
        // set current to the next node
        current = current->next;
    }

    // if it is last node or current's value is bigger than
    if (current == NULL || current->data > value)
    {
        // create new node
        list_node* temp = (list_node*)malloc(sizeof(list_node));
        // set new node's value to parameter
        temp->data = value;
        // set new node's next to current
        temp->next = current;
        // if previous is null, then it means we did not iterate
        // in list, so it must be list's head
        if (previous == NULL)
        {
            head = temp;
        }
        // else set previous node's next to new node
        else
        {
            previous->next = temp;
        }    
    }
    else
    {
        // did not insert
        return 0;
    }
    // inserted
    return 1;
}

int Delete(int value)
{
    // first point list's head with current variable
    list_node* current = head;
    // and store previous node's data
    list_node* previous = NULL;
    // iteration in list node to find where we will insert
    // the value (ascending order)
    while (current != NULL && current->data < value)
    {
        // set previous to current
        previous = current;
        // set current to the next node
        current = current->next;
    }
    // if current node is not null and its data is equal to parameter
    if (current != NULL && current->data == value)
    {
        // if previous is null, then it means we did not iterate
        // in list, so it must be list's head
        if (previous == NULL)
        {
            // now list's head is head's next node
            head = current->next;
            free(current);
        }
        else
        {
            // now previous's next is not pointing to current node
            // it must point current's next.
            previous->next = current->next;
            free(current);
        }    
    }
    else
    {
        // did not delete
        return 0;
    }
    // deleted
    return 1;
}

int Search(int value)
{
    // first point list's head with current variable
    list_node* current = head;
    // iterate in list
    while (current != NULL)
    {
        // if current node's data is equal to parameter
        if (current->data == value)
        {
            // found
            return 1;
        }
        // set current to the next node
        current = current->next;
    }
    // did not found
    return 0;
}

void Task_enqueue(int task_num, int task_type, int value)
{
    //printf("Task: %d, Type: %d, Value: %d\n", task_num, task_type, value);
    // new task node is created
    task_node* temp = (task_node*)malloc(sizeof(task_node));
    temp->task_num = task_num;
    temp->task_type = task_type;
    temp->value = value;
    temp->next = NULL;

    // if last_task is null, then it means queue is empty
    if (last_task == NULL)
    {
        // set first and last task to new task
        first_task = temp;
        last_task = temp; 
    }
    // queue is not empty
    else
    {
        // add to last
        last_task->next = temp;
        // now last task is new node
        last_task = temp;
    }
}

int Task_dequeue(long my_rank, int* task_num, int* task_type, int* value_p)
{
    // take first task
    task_node *tmp_first = first_task;
    // if it is null then queue is empty
    if (tmp_first == NULL)
    {
        printf("Queue is empty!!\n");
        return 0;
    }
    
    // set parameters to node's corresponding values
    *task_num = tmp_first->task_num;
    *task_type = tmp_first->task_type;
    *value_p = tmp_first->value;

    // if there is one node, we need to set last node too
    if (last_task == first_task){
        last_task = last_task->next;
    }

    // set first task to its next task
    first_task = first_task->next;

    free(tmp_first);
    return 1;
}

void Task_queue(int n)
{
    // generate task amount of given parameter
    for (int i = 0; i < n; i++)
    {
        // lock mutex because we don't want another
        // queue operation
        pthread_mutex_lock(&thread_mutex);
        // enqueue new task
        // rand() % n creates integer in [0,n) 
        Task_enqueue(i, rand() % 3, (rand()%500)+1);
        // after enqueue signal a thread
        pthread_cond_signal(&thread_cond);
        // unlock mutex
        pthread_mutex_unlock(&thread_mutex);
    }
    // after generating task set global variable
    all_tasks_generated = 1;
    // broadcast all threads
    pthread_cond_broadcast(&thread_cond);
}

void* work(void* my_rank)
{
    while (1)
    {
        // lock mutex
        pthread_mutex_lock(&thread_mutex);
		// while there is no task and all tasks is not generated
        // condition wait
        // after task is created, main thread sends signal and 
        // first task will not be null, so while condition will be broke
        // so thread will execute
        while(first_task == NULL && !all_tasks_generated){
            pthread_cond_wait(&thread_cond, &thread_mutex);
		}
        // if there is no task after thread wakes
        // then it means there will be no task, so terminate thread
        if (first_task == NULL )
        {
            pthread_mutex_unlock(&thread_mutex);
			pthread_exit(NULL);
        }
        
        // parameters to give Task_dequeue function
        long rank = (long) my_rank;
        int task_num, task_type, value;

        Task_dequeue(rank, &task_num, &task_type, &value);

        // checking task type
        switch (task_type)
        {
            // task type is insert
            case 0:
                // if inserted
                if (Insert(value))
                {
                    printf("Thread %ld: task %d: %d is inserted\n",
                        rank, task_num, value);
                }
                // if not inserted
                else
                {
                    printf("Thread %ld: task %d: %d cannot be inserted\n",
                        rank, task_num, value);
                }
                break;
            // task type is delete
            case 1:
                // if deleted
                if (Delete(value))
                {
                    printf("Thread %ld: task %d: %d is deleted\n",
                        rank, task_num, value);
                }
                // if not deleted
                else
                {
                    printf("Thread %ld: task %d: %d cannot be deleted\n",
                        rank, task_num, value);
                }
                break;
            // task type is search
            case 2:
                // if found
                if (Search(value))
                {
                    printf("Thread %ld: task %d: %d is found\n",
                        rank, task_num, value);
                }
                // if not found
                else
                {
                    printf("Thread %ld: task %d: %d is not found\n",
                        rank, task_num, value);
                }
                break;
            default:
                break;
        }
        // unlock mutex for other threads
        pthread_mutex_unlock(&thread_mutex);
    }
    
}

void print_list()
{
    printf("main: Final List:\n");
    // take first node
    list_node *tmp = head;
    while (tmp != NULL)
    {
        // if it is last node
        if (tmp->next == NULL)
        {
            // print it and break loop
            printf("%d\n", tmp->data);
            break;                    
        }
        // otherwise print it and skip to next node
        printf("%d ", tmp->data);
        tmp = tmp->next;
    }
}