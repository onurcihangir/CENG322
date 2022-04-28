#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

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

list_node* head = NULL;
task_node* first_task = NULL;
task_node* last_task = NULL;

int all_tasks_generated = 0;
int thread_awaken = 0;

pthread_mutex_t thread_mutex;
//pthread_mutex_t queue_mutex;
pthread_cond_t task_cond;

int main(int argc, char* argv[])
{
    srand ( time(NULL) );

    char *thread_num = argv[1];
    int thread_number = atoi(thread_num);

    char *task_num = argv[2];
    int task_number = atoi(task_num);

    //pthread_mutex_init(&queue_mutex, NULL);
    pthread_mutex_init(&thread_mutex, NULL);
    pthread_cond_init(&task_cond, NULL);

    struct timeval check_point;
    gettimeofday(&check_point, NULL);
    long beginning_time_in_usec = (long) check_point.tv_sec * SEC_TO_USEC_FACTOR +  (long) check_point.tv_usec;

    pthread_t tids[thread_number];
    for(int i = 0; i < thread_number; i++) {
        pthread_create(&tids[i], NULL, work, (void*)(long) i);
    }
    
    Task_queue(task_number);

    for(int i = 0; i < thread_number; i++) {
        pthread_join(tids[i], NULL);
    }

    print_list();

    gettimeofday(&check_point, NULL);
    long end_time_in_usec = (long) check_point.tv_sec * SEC_TO_USEC_FACTOR +  (long) check_point.tv_usec;
        
    pthread_mutex_destroy(&thread_mutex);
    pthread_cond_destroy(&task_cond);

    double elapsed_time_in_sec = (end_time_in_usec - beginning_time_in_usec) / (double) SEC_TO_USEC_FACTOR;
    printf("[INFO] Elapsed time in seconds: %f\n", elapsed_time_in_sec);  

    return 0;
}

int Insert(int value)
{
    list_node* current = head;
    list_node* previous = NULL;
    while (current != NULL && current->data < value)
    {
        previous = current;
        current = current->next;
    }

    if (current == NULL || current->data > value)
    {
        list_node* temp = (list_node*)malloc(sizeof(list_node));
        temp->data = value;
        temp->next = current;
        if (previous == NULL)
        {
            head = temp;
        }
        else
        {
            previous->next = temp;
        }    
    }
    else
    {
        return 0;
    }
    return 1;
}

int Delete(int value)
{
    list_node* current = head;
    list_node* previous = NULL;
    while (current != NULL && current->data < value)
    {
        previous = current;
        current = current->next;
    }
    if (current != NULL && current->data == value)
    {
        if (previous == NULL)
        {
            head = current->next;
            free(current);
        }
        else
        {
            previous->next = current->next;
            free(current);
        }    
    }
    else
    {
        return 0;
    }
    return 1;
}

int Search(int value)
{
    list_node* current = head;
    while (current != NULL)
    {
        if (current->data == value)
        {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void Task_enqueue(int task_num, int task_type, int value)
{
    //printf("Task: %d, Type: %d, Value: %d\n", task_num, task_type, value);
    task_node* temp = (task_node*)malloc(sizeof(task_node));
    temp->task_num = task_num;
    temp->task_type = task_type;
    temp->value = value;
    temp->next = NULL;

    if (last_task == NULL)
    {
        first_task = temp;
        last_task = temp; 
    }
    else
    {
        last_task->next = temp;
        last_task = temp;
    }
}

int Task_dequeue(long my_rank, int* task_num, int* task_type, int* value_p)
{
    task_node *tmp_first = first_task;
    if (tmp_first == NULL)
    {
        printf("Queue is empty!!\n");
        return 0;
    }
    
    *task_num = tmp_first->task_num;
    *task_type = tmp_first->task_type;
    *value_p = tmp_first->value;

    if (last_task == first_task){
        last_task = last_task->next;
    } //last task

    first_task = first_task->next;

    free(tmp_first);
    return 1;
}

void Task_queue(int n)
{
    //pthread_mutex_lock(&queue_mutex);

    for (int i = 0; i < n; i++)
    {
        Task_enqueue(i, rand() % 3, (rand()%500)+1);
        //usleep(10);
        pthread_cond_signal(&task_cond);
        usleep(10);
    }
    //pthread_mutex_unlock(&queue_mutex);
    //usleep(1000);

    all_tasks_generated = 1;

    pthread_cond_broadcast(&task_cond); 
}

void* work(void* my_rank)
{

    while (1)
    {
        pthread_mutex_lock(&thread_mutex);
        pthread_cond_wait(&task_cond, &thread_mutex);
		
        if(first_task == NULL){
			pthread_mutex_unlock(&thread_mutex);
			break;
		}

        long rank = (long) my_rank;

        int task_num, task_type, value;

        Task_dequeue(rank, &task_num, &task_type, &value);

        switch (task_type)
        {
            case 0:
                if (Insert(value))
                {
                    printf("Thread %ld: task %d: %d is inserted\n",
                        rank, task_num, value);
                }
                else
                {
                    printf("Thread %ld: task %d: %d cannot be inserted\n",
                        rank, task_num, value);
                }
                break;
            case 1:
                if (Delete(value))
                {
                    printf("Thread %ld: task %d: %d is deleted\n",
                        rank, task_num, value);
                }
                else
                {
                    printf("Thread %ld: task %d: %d cannot be deleted\n",
                        rank, task_num, value);
                }
                break;
            case 2:
                if (Search(value))
                {
                    printf("Thread %ld: task %d: %d is found\n",
                        rank, task_num, value);
                }
                else
                {
                    printf("Thread %ld: task %d: %d is not found\n",
                        rank, task_num, value);
                }
                break;
            default:
                break;
        }
        pthread_mutex_unlock(&thread_mutex);
    }
    
}

void print_list()
{
    printf("main: Final List:\n");
    list_node *tmp = head;
    while (tmp != NULL)
    {
        if (tmp->next == NULL)
        {
            printf("%d\n", tmp->data);
            break;                    
        }
        printf("%d ", tmp->data);
        tmp = tmp->next;
    }
}