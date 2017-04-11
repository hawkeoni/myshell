mod.o: mod.c mod.h
	gcc -g -Wall -c mod.c -o mod.o

shell: shell.c mod.o
	gcc -g -Wall shell.c mod.o -o shell
