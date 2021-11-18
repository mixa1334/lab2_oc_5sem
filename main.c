#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>

sem_t *writer_s;
sem_t *reader_s;
char *symbol;

void readProcess(const char *file_name);
void writeProcess(const char *file_name);
void cleanUp(int shmid);

int main()
{
    key_t shmkey;
    int shmid;

    shmkey = ftok("/dev/null", 5);
    shmid = shmget(shmkey, sizeof (char), 0644 | IPC_CREAT);
    if (shmid < 0)
    {
        perror("shmget\n");
        exit(EXIT_FAILURE);
    }

    symbol = (char*) shmat(shmid, NULL, 0);
    *symbol = EOF;

    writer_s = sem_open("writer_sem", O_CREAT | O_EXCL, 0644, 0);
    reader_s = sem_open("reader_sem", O_CREAT | O_EXCL, 0644, 1);

    pid_t reader = fork();
    if(reader > 0)
    {
        pid_t writer = fork();
        if(writer > 0)
        {
            cleanUp(shmid);
        }
        else if (writer == 0)
        {
            writeProcess("output.txt");
        }
        else
        {
            cleanUp(shmid);
            exit(EXIT_FAILURE);
        }
        
    }
    else if(reader == 0)
    {
        readProcess("input.txt");
    }
    else
    {
        cleanUp(shmid);
        exit(EXIT_FAILURE);
    }

    return 0;
}

void readProcess(const char *file_name)
{
    FILE *file;
    file = fopen(file_name, "r");

    if (file != NULL)
    {
        while (1)
        {
            sem_wait(reader_s);
            *symbol = fgetc(file);
            if(*symbol == EOF)
            {
                sem_post(writer_s);
                break;
            }
            printf("symbol from file - %c\n", *symbol);
            sem_post(writer_s);
        }
        fclose(file);
    }
}

void writeProcess(const char *file_name)
{
    FILE *file;
    file = fopen(file_name, "w");

    if (file != NULL)
    {
        while (1)
        {
            sem_wait(writer_s);
            if(*symbol == EOF){
                sem_post(reader_s);
                break;
            }
            fputc(*symbol, file);
            printf("symbol to write - %c\n", *symbol);
            sem_post(reader_s);
        }
        fclose(file);   
    }
}

void cleanUp(int shmid)
{
    pid_t pid;
    while (pid = waitpid(-1, NULL, 0))
    {
        if (errno == ECHILD)
            break;
    }

    shmdt(symbol);
    shmctl(shmid, IPC_RMID, 0);

    sem_unlink("writer_sem");  
    sem_unlink("reader_sem"); 
    sem_close(writer_s);
    sem_close(reader_s);
}