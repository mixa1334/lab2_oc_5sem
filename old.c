#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//pthread_mutex_t lock;
sem_t writer_s;
sem_t reader_s;

char symbol;

void* fileReader(void* arg);

void* fileWriter(void* arg);

int main(){
    symbol = EOF;

    //pthread_mutex_init(&lock, NULL);
    sem_init(&writer_s, 0, 0);
    sem_init(&reader_s, 0, 1);

    pthread_t reader_thread, writer_thread;

    pthread_create(&reader_thread,NULL,fileReader,NULL);
    sleep(2);
    pthread_create(&writer_thread,NULL,fileWriter,NULL);

    pthread_join(reader_thread, NULL);
    pthread_join(writer_thread, NULL);

    sem_destroy(&reader_s);
    sem_destroy(&writer_s);

    return 0;
}

void* fileReader(void* arg)
{
    FILE *file;
    file = fopen("input.txt", "r");

    if (file != NULL)
    {
        while (1)
        {
            sem_wait(&reader_s);
            symbol = fgetc(file);
            if(symbol == EOF)
            {
                sem_post(&writer_s);
                break;
            }
            printf("symbol from file - %c\n", symbol);
            sem_post(&writer_s);
        }
        fclose(file);
    }
}

void* fileWriter(void* arg)
{
    FILE *file;
    file = fopen("output.txt", "w");

    if (file != NULL)
    {
        while (1)
        {
            sem_wait(&writer_s);
            if(symbol == EOF){
                sem_post(&reader_s);
                break;
            }
            fputc(symbol, file);
            printf("symbol to write - %c\n", symbol);
            sem_post(&reader_s);
        }
        fclose(file);   
    }
}