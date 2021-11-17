main: main.o
	gcc main.o -o main -pthread
main.o: main.c
	gcc -c main.c -pthread
clean:
	rm -f main main.o