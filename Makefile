libcguieng.a: cguieng.o
	ar -rcv $@ $^

libcguieng.so: cguieng.o
	gcc -shared -o $@ $^

cguieng.o: cguieng.h cguieng.c
	gcc -c -fPIC cguieng.c -o $@

set_env: set_lib
	export LD_LIBRARY_PATH

set_lib:
	LD_LIBRARY_PATH=../ConsoleGUI ; echo $$LD_LIBRARY_PATH

clean:
	rm -f *.a *.so *.o test_* test