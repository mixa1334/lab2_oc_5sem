#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t lock;
char symbol;

int main(){
    pthread_mutex_init(&lock, NULL);

    if(readFile("input.txt") != 0)
    {
        printf("cant read from file\n");
    }
    printf("\n\n");
    if(writeFile("output.txt") != 0)
    {
        printf("cant write to file\n");
    }

    return 0;
}

int readFile(char *fileName)
{
    printf("file -> %s\n", fileName);
    FILE *file;
    file = fopen(fileName, "r");
    char c;

    if (file == NULL)
    {
        return -1;
    }


    while ((c = fgetc(file)) != EOF)
    {
        printf("%c", c);
        symbol = c;
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

    for (size_t i = 0; i < 5; i++)
    {
        printf("%c", symbol);
        fputc(symbol, file);
    }
    
    fclose(file);
    
    return 0;
}