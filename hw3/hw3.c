#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

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

list_node* head = NULL;
task_node* first_task = NULL;
task_node* last_task = NULL;

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