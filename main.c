#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t lock;
sem_t writer;
sem_t reader;

char symbol;


int main(){

    pthread_mutex_init(&lock, NULL);
    sem_init(&writer, 0, 1);
    sem_init(&reader, 0, 1);

    pthread_t reader_thread, writer_thread;



    pthread_join(reader_thread, NULL);
    pthread_join(writer_thread, NULL);


    // if(readFile("input.txt") != 0)
    // {
    //     printf("cant read from file\n");
    // }
    // printf("\n\n");
    // if(writeFile("output.txt") != 0)
    // {
    //     printf("cant write to file\n");
    // }

    return 0;
}

int readFile(char *fileName)
{
    printf("file -> %s\n", fileName);
    FILE *file;
    file = fopen(fileName, "r");

    if (file == NULL)
    {
        return -1;
    }

    while (1)
    {
        symbol = fgetc(file);
        if(symbol == EOF){
            break;
        }
        printf("%c", symbol);
    }

    fclose(file);
    
    return 0;
}

int writeFile(char *fileName)
{
    printf("file -> %s\n", fileName);
    FILE *file;
    file = fopen("output.txt", "w");

    if (file == NULL)
    {
        return -1;
    }

    while (1)
    {
        printf("%c", symbol);
        if(symbol == EOF){
            break;
        }
        fputc(symbol, file);
    }
    
    fclose(file);
    
    return 0;
}