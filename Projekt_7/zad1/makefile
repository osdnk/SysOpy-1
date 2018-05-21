
CC = gcc
CFLAGS = -g -O2 -std=c11 -static -Wall -Isrc -rdynamic -fomit-frame-pointer

all: $(patsubst %.c, %.out, $(wildcard *.c))

%.out: %.c
	$(CC) $(CFLAGS) $< -o $(@:.out=) -lrt

# clean: $(patsubst %.c, %.clean, $(wildcard *.c))

# %.clean:
	# rm -f $(@:.clean=)
clean:
	# ipcrm -s $(ipcs -s | grep 666 | tr -s ' ' | cut -d ' ' -f 2)
