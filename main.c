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
#include <signal.h>

sem_t *writer_s;
sem_t *reader_s;
char *symbol;
pid_t rpid;
pid_t wpid;

void readProcess(const char *file_name);
void writeProcess(const char *file_name);
void cleanUp(int shmid);
void sig_usr();

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

    writer_s = sem_open("writer_sem", O_CREAT | O_EXCL, 0644, -1);
    reader_s = sem_open("reader_sem", O_CREAT | O_EXCL, 0644, 0);
    wpid = -1;
    rpid = -1;

    pid_t rpid = fork();
    if(rpid > 0)
    {
        pid_t wpid = fork();
        if(wpid > 0)
        {
            cleanUp(shmid);
        }
        else if (wpid == 0)
        {
            writeProcess("output.txt");
        }
        else
        {
            cleanUp(shmid);
            exit(EXIT_FAILURE);
        }
        
    }
    else if(rpid == 0)
    {
        readProcess("_input.txt");
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

    if (file == NULL)
    {
        kill(getppid(), SIGUSR1);
        pause();
    }
    
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

void writeProcess(const char *file_name)
{
    FILE *file;
    file = fopen(file_name, "w");

    if (file == NULL)
    {    
        kill(getppid(), SIGUSR1);
        pause();
    }
    
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

void cleanUp(int shmid)
{
    signal(SIGUSR1, (void (*)(int))sig_usr);
    pid_t pid;
    sem_post(reader_s);
    sem_post(writer_s);
    while ((pid = wait(NULL)) > 0){}

    printf("start clean");

    shmdt(symbol);
    shmctl(shmid, IPC_RMID, 0);

    sem_unlink("writer_sem");  
    sem_unlink("reader_sem"); 
    sem_close(writer_s);
    sem_close(reader_s);
}

void sig_usr(int sig)
{
    if(sig == SIGUSR1)
    {    
        if(rpid > 0)
        {
            printf("kill r");
            kill(rpid, SIGKILL);
        }
        if(wpid > 0)
        {
            printf("kill w");
            kill(wpid, SIGKILL);
        }
    }
}