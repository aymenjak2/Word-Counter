COMPILER = gcc
FLAGS = -g -Wall -std=c99
EXEC = word-counter

all:word-counter.c
        $(COMPILER) word-counter.c -o $(EXEC) $(FLAGS)

clean:
        rm -f *.o $(EXEC)
