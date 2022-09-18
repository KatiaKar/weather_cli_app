CC = g++
CFLAGS = -Wall -g
 
# ****************************************************
# Targets needed to bring the executable up to date
 
main: main.o Command.o
    $(CC) $(CFLAGS) -o main main.o Command.o
 
# The main.o target can be written more simply
 
main.o: main.cpp Command.h
    $(CC) $(CFLAGS) -c main.cpp