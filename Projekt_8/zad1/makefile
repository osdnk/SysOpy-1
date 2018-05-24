CC = gcc
CFLAGS = -g -O0 -std=c11 -static -Wall -Isrc -rdynamic -fomit-frame-pointer
all: compile test

compile:
	$(CC) $(CFLAGS) main.c -o main -lm -Wl,--whole-archive -lpthread -Wl,--no-whole-archive

test:
	./main 2 "./totem.pgm" "./filter.txt" "./out.pgm"

clean: $(patsubst %.c, %.clean, $(wildcard *.c))

%.clean:
	rm -f $(@:.clean=)
