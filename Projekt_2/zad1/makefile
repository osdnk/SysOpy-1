CC = gcc -Wall -fPIC -ggdb
LIB_NAME_1 = sys_ver
LIB_NAME_2 = lib_ver
MAIN_SRC = main

compile: static

static:
	# kompilacja zrodla
	$(CC)  -c $(LIB_NAME_1).c -o $(LIB_NAME_1).o 
	$(CC)  -c $(LIB_NAME_2).c -o $(LIB_NAME_2).o 

	# zbuduj biblioteke statyczna .a
	ar rcs lib$(LIB_NAME_1).a $(LIB_NAME_1).o 
	ar rcs lib$(LIB_NAME_2).a $(LIB_NAME_2).o 

	# linkowanie
	$(CC)  -static $(MAIN_SRC).c -L. -l $(LIB_NAME_1) -l $(LIB_NAME_2) -o $(MAIN_SRC)


clean:
	rm -f *~
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f $(MAIN_SRC)

clean_all: clean
	rm -f *.txt
	rm -f *.out

test: compile
	./main -F test.txt -n 1000 -s 512 -C cp_test.txt
	make clean