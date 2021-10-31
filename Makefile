CC = gcc
CFLAGS = -Wall -g
OBJ1 = project4.o 
OBJ2 = p4diff.o

all: project4 p4diff

project4: project4.o
	$(CC) $(CFLAGS) $(OBJ1) -o project4
	
project4.o: project4.c 
	$(CC) $(CFLAGS) -c project4.c
	
p4diff: p4diff.o
	$(CC) $(CFLAGS) $(OBJ2) -o p4diff
	
p4diff.o: p4diff.c 
	$(CC) $(CFLAGS) -c p4diff.c

.PHONY: clean
clean:
	rm *.o
