cc = gcc
CLAGS = -g -Wall
LDFLAGS =
LDLIBS =

chip8: chip8.o

chip8.o: chip8.c

.PHONY: clean

clean:
	rm -f *.o a.out core chip8

.PHONY: all
all:
	clean chip8
