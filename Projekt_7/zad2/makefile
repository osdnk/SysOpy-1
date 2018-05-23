CC = /usr/bin/gcc-6
CFLAGS = -g -O0 -std=c11 -static -Wall -Isrc -rdynamic -fomit-frame-pointer
all: barber client spawner

barber:
	$(CC) $(CFLAGS) props.c barber.c -o barber -Wl,--whole-archive -lrt -Wl,--no-whole-archive -Wl,--whole-archive -lpthread -Wl,--no-whole-archive

client:
	$(CC) $(CFLAGS) props.c client.c -o client -Wl,--whole-archive -lrt -Wl,--no-whole-archive -Wl,--whole-archive -lpthread -Wl,--no-whole-archive

spawner:
	$(CC) $(CFLAGS) spawner.c -o spawner

clean: $(patsubst %.c, %.clean, $(wildcard *.c))

%.clean:
	rm -f $(@:.clean=)