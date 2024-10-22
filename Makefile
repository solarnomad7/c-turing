CC := gcc
CFLAGS := -Wall -Wextra
BINARY := turing

$(BINARY): main.o machine.o parser.o
	$(CC) $(CFLAGS) parser.o machine.o main.o -o $(BINARY)
	rm -f *.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

machine.o: machine.c machine.h
	$(CC) $(CFLAGS) -c machine.c -o machine.o

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c -o parser.o

clean:
	rm -f $(BINARY) *.o
