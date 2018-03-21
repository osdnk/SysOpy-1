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
	rm -f *.txt
	
clean_all: clean
	rm -f *.out

test: compile
	./main -F test.txt -n 10000000 -s 4 -C cp_test.txt -t lib -o generate
	./main -F test.txt -n 10000000 -s 4 -C cp_test.txt -t lib -o copy
	./main -F test.txt -n 10000000 -s 4 -C cp_test_s.txt -t sys -o copy

	./main -F test.txt -n 30000000 -s 4 -C cp_test.txt -t lib -o generate
	./main -F test.txt -n 30000000 -s 4 -C cp_test.txt -t lib -o copy
	./main -F test.txt -n 30000000 -s 4 -C cp_test_s.txt -t sys -o copy

	./main -F test.txt -n 1000000 -s 512 -C cp_test.txt -t lib -o generate
	./main -F test.txt -n 1000000 -s 512 -C cp_test.txt -t lib -o copy
	./main -F test.txt -n 1000000 -s 512 -C cp_test_s.txt -t sys -o copy

	./main -F test.txt -n 3000000 -s 512 -C cp_test.txt -t lib -o generate
	./main -F test.txt -n 3000000 -s 512 -C cp_test.txt -t lib -o copy
	./main -F test.txt -n 3000000 -s 512 -C cp_test_s.txt -t sys -o copy

	./main -F test.txt -n 100000 -s 4096 -C cp_test.txt -t lib -o generate
	./main -F test.txt -n 100000 -s 4096 -C cp_test.txt -t lib -o copy
	./main -F test.txt -n 100000 -s 4096 -C cp_test_s.txt -t sys -o copy

	./main -F test.txt -n 300000 -s 4096 -C cp_test.txt -t lib -o generate
	./main -F test.txt -n 300000 -s 4096 -C cp_test.txt -t lib -o copy
	./main -F test.txt -n 300000 -s 4096 -C cp_test_s.txt -t sys -o copy

	./main -F test.txt -n 100000 -s 8192 -C cp_test.txt -t lib -o generate
	./main -F test.txt -n 100000 -s 8192 -C cp_test.txt -t lib -o copy
	./main -F test.txt -n 100000 -s 8192 -C cp_test_s.txt -t sys -o copy

	./main -F test.txt -n 300000 -s 8192 -C cp_test.txt -t lib -o generate
	./main -F test.txt -n 300000 -s 8192 -C cp_test.txt -t lib -o copy
	./main -F test.txt -n 300000 -s 8192 -C cp_test_s.txt -t sys -o copy


	./main -F test.txt -n 1000 -s 4 -t lib -o generate
	./main -F test.txt -n 1000 -s 4 -t lib -C cp_test.txt -o copy
	./main -F test.txt -n 1000 -s 4 -t sys -o sort
	./main -F test.txt -n 1000 -s 4 -t lib -o sort

	./main -F test.txt -n 5000 -s 4 -t lib -o generate
	./main -F test.txt -n 5000 -s 4 -t lib -C cp_test.txt -o copy
	./main -F test.txt -n 5000 -s 4 -t sys -o sort
	./main -F cp_test.txt -n 5000 -s 4 -t lib -o sort

	./main -F test.txt -n 1000 -s 512 -t lib -o generate
	./main -F test.txt -n 1000 -s 512 -t lib -C cp_test.txt -o copy
	./main -F test.txt -n 1000 -s 512 -t sys -o sort
	./main -F cp_test.txt -n 1000 -s 512 -t lib -o sort

	./main -F test.txt -n 5000 -s 512 -t lib -o generate
	./main -F test.txt -n 5000 -s 512 -t lib -C cp_test.txt -o copy
	./main -F test.txt -n 5000 -s 512 -t sys -o sort
	./main -F cp_test.txt -n 5000 -s 512 -t lib -o sort

	./main -F test.txt -n 1000 -s 4096 -t lib -o generate
	./main -F test.txt -n 1000 -s 4096 -t lib -C cp_test.txt -o copy
	./main -F test.txt -n 1000 -s 4096 -t sys -o sort
	./main -F cp_test.txt -n 1000 -s 4096 -t lib -o sort

	./main -F test.txt -n 5000 -s 4096 -t lib -o generate
	./main -F test.txt -n 5000 -s 4096 -t lib -C cp_test.txt -o copy
	./main -F test.txt -n 5000 -s 4096 -t sys -o sort
	./main -F cp_test.txt -n 5000 -s 4096 -t lib -o sort

	./main -F test.txt -n 1000 -s 8192 -t lib -o generate
	./main -F test.txt -n 1000 -s 8192 -t lib -C cp_test.txt -o copy
	./main -F test.txt -n 1000 -s 8192 -t sys -o sort
	./main -F cp_test.txt -n 1000 -s 8192 -t lib -o sort

	./main -F test.txt -n 5000 -s 8192 -t lib -o generate
	./main -F test.txt -n 5000 -s 8192 -t lib -C cp_test.txt -o copy
	./main -F test.txt -n 5000 -s 8192 -t sys -o sort
	./main -F cp_test.txt -n 5000 -s 8192 -t lib -o sort

	make clean