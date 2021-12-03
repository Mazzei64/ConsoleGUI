Drun: build_dynamic
	clear ; ./test_dynamic

Srun: build_static
	clear ; ./test_static

build: clean test_dynamic test_static

build_dynamic: test_dynamic set_env

build_static: test_static

lbuild: libcguieng.so libcguieng.a

test_static: test.c libcguieng.a
	gcc -g $^ -o $@

libcguieng.a: cguieng.o
	ar -rcv $@ $^

test_dynamic: test.c libcguieng.so
	gcc test.c -L. -lcguieng -o $@

libcguieng.so: cguieng.o
	gcc -shared -o $@ $^

cguieng.o: cguieng.h cguieng.c
	gcc -c -fPIC cguieng.c -o $@

test: test.c
	gcc -g test.c -o $@

set_env: set_lib
	export LD_LIBRARY_PATH

set_lib:
	LD_LIBRARY_PATH=../ConsoleGUI ; echo $$LD_LIBRARY_PATH

clean:
	rm -f *.a *.so *.o test_* test