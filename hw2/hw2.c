#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_CHAR 100
#define MAX_ARG 10
#define HISTORY_CAP 10

// node structure for history
// it has string value and next node
typedef struct Node
{
    char *str;
    struct Node *next;
} node;

// queue structure for history
typedef struct Queue
{
    node *front, *rear;
} Queue;

Queue *create_queue()
{
    Queue *q = (Queue *)malloc(sizeof(node));
    q->front = NULL;
    q->rear = NULL;
    return q;
}

// return length of queue
int get_length(Queue *q)
{
    int count = 0;
    // first initialize front node to temp
    node *temp = q->front;
    // check if node is empty
    while (temp != NULL)
    {
        count++;
        // skip to next node
        temp = temp->next;
    }
    return count;
}

void dequeue(Queue *q)
{
    // if front node is empty then empty queue
    if (q->front == NULL)
    {
        return;
    }
    // set temp to first node
    node *temp = q->front;
    // set first node to next node
    q->front = q->front->next;

    // if front node is empty
    // it means queue is empty, then set rear to null
    if (q->front == NULL)
    {
        q->rear = NULL;
    }

    free(temp);
}

void enqueue(Queue *q, char *str)
{
    // create new node with given string
    node *temp = (node *)malloc(sizeof(node));
    temp->str = str;
    temp->next = NULL;
    // if last node is empty, it means queue is empty
    // set first and last node to new node
    if (q->rear == NULL)
    {
        q->front = temp;
        q->rear = temp;
        return;
    }
    // if queue length is equal to capacity
    // then dequeue the first node
    // and add new node to the last
    else if (get_length(q) == HISTORY_CAP)
    {
        dequeue(q);
        q->rear->next = temp;
        q->rear = temp;
    }
    // otherwise add node to last node's next
    else
    {
        q->rear->next = temp;
        q->rear = temp;
    }
}

int bye(char **args)
{
    // bye command simply returns 0 to finish the loop
    return 0;
}

int cd(char **args)
{
    // check if second argument is NULL or empty string
    if (args[1] == NULL || strcmp(args[1], "") == 0)
    {
        chdir(getenv("HOME"));
    }
    else
    {
        // check directory is exist
        // https://codeforwin.org/2018/03/c-program-check-file-or-directory-exists-not.html
        struct stat stats;
        stat(args[1], &stats);
        if (S_ISDIR(stats.st_mode))
        {
            // if exist then change directory
            chdir(args[1]);
        }
        else
        {
            // if not exist print error
            fprintf(stderr, "%s: No such file or directory\n", args[1]);
        }
    }
    // keep going in the loop
    return 1;
}

int dir(char **args)
{
    char cwd[256];
    // call getcwd method
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s\n", cwd);
    }
    else
    {
        fprintf(stderr, "getcwd() error\n");
        return 1;
    }
    return 1;
}

int history(Queue *q)
{
    int count = 0;
    // set temp to first node of queue
    node *temp = q->front;
    // if temp is not null
    while (temp != NULL)
    {
        count++;
        // print count and command string
        printf("[%d] %s\n", count, temp->str);
        // skip to next node
        temp = temp->next;
    }
    return 1;
}

int other_commands(char **args)
{
    int status;
    pid_t pid, wpid;
    int ampersand = 0;
    int i = 0;
    // we need this while block for
    // changing the '&' in the end of command
    // because we do not want to give '&' as an
    // argument in execvp().
    while (args[i] != NULL)
    {
        // check if argument is '&'
        if (strcmp(args[i], "&") == 0)
        {
            // there is ampersand
            ampersand = 1;
            // change ampersand in args array
            args[i] = '\0';
        }
        i++;
    }
    // fork error
    if ((pid = fork()) < 0)
    {
        fprintf(stderr, "*** ERROR: forking child process failed\n");
        exit(1);
    }
    // child process
    else if (pid == 0)
    {
        if (execvp(args[0], args) < 0)
        { // If it is not built in function that defined from us ,execute it in child
            fprintf(stderr, "Error: execvp() failed\n");
        }
        exit(1);
    }
    // parent process
    else
    {
        // if there is not ampersand
        if (ampersand == 0)
        {
            // wait for the child process
            while ((wpid = wait(&status)) > 0);
        }
    }
    return 1;
}

int exec_commands(char **args, Queue *q)
{
    if (strcmp(args[0], "bye") == 0)
    {
        return bye(args);
    }
    else if (strcmp(args[0], "cd") == 0)
    {
        return cd(args);
    }
    else if (strcmp(args[0], "dir") == 0)
    {
        return dir(args);
    }
    else if (strcmp(args[0], "history") == 0)
    {
        return history(q);
    }
    else
    {
        return other_commands(args);
    }
}

char *read_args()
{
    char *str = malloc(sizeof(char) * MAX_CHAR);
    if (!str)
    {
        fprintf(stderr, "Error: malloc error\n");
        exit(EXIT_FAILURE);
    }

    fgets(str, MAX_CHAR, stdin);
    str[strcspn(str, "\n")] = 0;
    return str;
}

char **parse_args(char *line)
{
    char *token;
    int index = 0;
    // create array of strings
    char **args = malloc(sizeof(char *) * MAX_ARG);
    if (!args)
    {
        fprintf(stderr, "Error: malloc error\n");
        exit(EXIT_FAILURE);
    }
    token = strtok(line, " ");
    while (token != NULL)
    {
        // https://stackoverflow.com/a/28462221
        // strcspn function returns number of chars until
        // it sees the given string.
        token[strcspn(token, "\n")] = 0;
        args[index] = token;
        index++;
        token = strtok(NULL, " ");
    }

    return args;
}

void shell()
{
    int status = 1;
    // queue for history
    Queue *q = create_queue();
    while (status)
    {
        printf("myshell>");
        // read line
        char *line = read_args();
        // create copy of line
        // otherwise when we parse line
        // command in history queue will change
        char *str = malloc(sizeof(char) * MAX_CHAR);
        memcpy(str, line, sizeof(char) * MAX_CHAR);
        // add copy of line to history queue
        enqueue(q, str);
        // parse arguments
        char **args = parse_args(line);
        // execute built-in commands
        status = exec_commands(args, q);
    }
}

int main(int argc, char *argv[])
{
    shell();

    return 0;
}