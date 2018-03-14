CC = gcc -Wall
LIB_NAME = lib1
MAIN_SRC = main

compile: static shared dynamic

static:
	# kompilacja zrodla
	$(CC) -c $(LIB_NAME).c -o $(LIB_NAME).o 

	# zbuduj biblioteke statyczna .a
	ar rcs lib$(LIB_NAME).a $(LIB_NAME).o 

	# linkowanie
	$(CC) -fPIC -static $(MAIN_SRC).c -L. -l $(LIB_NAME) -o $(MAIN_SRC)_static 

shared:
	# zbuduj biblioteke wspoldzielona .so
	$(CC) -fPIC -c $(LIB_NAME).c -o $(LIB_NAME).o 
	$(CC) -fPIC -shared -o lib$(LIB_NAME).so $(LIB_NAME).o 

	# linkowanie
	$(CC) -fPIC -Wl,-rpath=. -L. -o $(MAIN_SRC)_shared $(MAIN_SRC).c -l$(LIB_NAME) 

dynamic:
	# zbuduj biblioteke wspoldzielona .so
	$(CC) -fPIC -c $(LIB_NAME).c -o $(LIB_NAME).o 
	$(CC) -Wl,-rpath=. -fPIC -shared -o lib$(LIB_NAME).so $(LIB_NAME).o 

	# linkowanie
	$(CC) -fPIC -L. -o $(MAIN_SRC)_dynamic $(MAIN_SRC).c -ldl -D DYNAMIC 

clean:
	rm -f *~
	rm -f *.o
	rm -f *.a
	rm -f *.so

clean_all: clean
	rm -f $(MAIN_SRC)_dynamic
	rm -f $(MAIN_SRC)_shared
	rm -f $(MAIN_SRC)_static

test: compile
	echo >> raport3a.txt
	echo STATIC LIB ADD_REMOVE >> raport3a.txt
	./$(MAIN_SRC)_static 500000 500 dynamic c f 10000 ar 100000
	./$(MAIN_SRC)_static 500000 500 static c f 10000 ar 100000
	echo >> raport3a.txt
	echo SHARED LIB ADD_REMOVE >> raport3a.txt
	./$(MAIN_SRC)_shared 500000 500 dynamic c f 10000 ar 100000
	./$(MAIN_SRC)_shared 500000 500 static c f 10000 ar 100000
	echo >> raport3a.txt
	echo DYNAMIC LIB ADD_REMOVE >> raport3a.txt
	./$(MAIN_SRC)_dynamic 500000 500 dynamic c f 10000 ar 100000
	./$(MAIN_SRC)_dynamic 500000 500 static c f 10000 ar 100000
	echo >> raport3a.txt
	echo STATIC LIB ADD_REMOVE_CROSS >> raport3a.txt
	./$(MAIN_SRC)_static 500000 500 dynamic c ar 100000 arc 100000
	./$(MAIN_SRC)_static 500000 500 static c ar 100000 arc 100000
	echo >> raport3a.txt
	echo SHARED LIB ADD_REMOVE_CROSS >> raport3a.txt
	./$(MAIN_SRC)_shared 500000 500 dynamic c ar 100000 arc 100000
	./$(MAIN_SRC)_shared 500000 500 static c ar 100000 arc 100000
	echo >> raport3a.txt
	echo DYNAMIC LIB ADD_REMOVE_CROSS >> raport3a.txt
	./$(MAIN_SRC)_dynamic 500000 500 dynamic c ar 100000 arc 100000
	./$(MAIN_SRC)_dynamic 500000 500 static c ar 100000 arc 100000
	make clean_all