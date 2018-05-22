CC = gcc
CFLAGS = -g -O2 -std=c11 -static -Wall -Isrc -rdynamic -fomit-frame-pointer
all: barber client spawner

barber:
	$(CC) $(CFLAGS) props.c barber.c -o barber -lrt -pthread

client:
	$(CC) $(CFLAGS) props.c client.c -o client -lrt -pthread

spawner:
	$(CC) $(CFLAGS) spawner.c -o spawner

clean: $(patsubst %.c, %.clean, $(wildcard *.c))

%.clean:
	rm -f $(@:.clean=)