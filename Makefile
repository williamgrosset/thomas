.phony setup:
setup: shell

shell: shell.c
	gcc shell.c -lreadline -o shell.o

run: shell.o
	./shell.o

.PHONY clean:
clean:
	rm -rf *.o *.exe
