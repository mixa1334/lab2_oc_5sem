main: main.o
	gcc main.o -o main -pthread -lrt
main.o: main.c
	gcc -c main.c -pthread -lrt
clean:
	rm -f main main.o