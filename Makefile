.phony setup:
setup: mts

mts: mts.c
	gcc mts.c -o mts.o

run: mts.o
	./mts.o

.PHONY clean:
clean:
	rm -rf *.o *.exe
