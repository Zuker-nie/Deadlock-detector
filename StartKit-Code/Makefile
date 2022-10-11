# makefile for scheduling program        
CC=gcc
DEPS = banker.h list.h process.h activity.h
OBJ = banker.o list.o
CFLAGS=-Wall
STD_FLAG=-std=c99


banker.o: banker.c banker.h 
	$(CC) $(CFLAGS) banker.c -lm list.c $(STD_FLAG)  -o banker

debug: banker.c banker.h
	$(CC) $(CFLAGS) -g banker.c list.c $(STD_FLAG) -o banker

list.o: list.c list.h process.h
	$(CC) $(CFLAGS) -c list.c

clean:
	rm -rf *.o    

