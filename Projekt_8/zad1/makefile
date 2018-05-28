CC = gcc
CFLAGS = -g -O0 -std=c11 -static -Wall -Isrc -rdynamic -fomit-frame-pointer
all: compile test

compile:
	$(CC) $(CFLAGS) main.c -o main -lm -Wl,--whole-archive -lpthread -Wl,--no-whole-archive

test:
	# valgrind -v --track-origins=yes --keep-stacktraces=alloc-and-free
	./main 1 "./totem.pgm" "./edge_detection.txt" "./out_e1.pgm"
	./main 2 "./totem.pgm" "./edge_detection.txt" "./out_e2.pgm"
	./main 4 "./totem.pgm" "./edge_detection.txt" "./out_e3.pgm"
	./main 1 "./totem.pgm" "./32_32_filter.txt" "./out_1.pgm"
	./main 2 "./totem.pgm" "./32_32_filter.txt" "./out_2.pgm"
	./main 4 "./totem.pgm" "./32_32_filter.txt" "./out_4.pgm"

clean: $(patsubst %.c, %.clean, $(wildcard *.c))

%.clean:
	rm -f $(@:.clean=)
