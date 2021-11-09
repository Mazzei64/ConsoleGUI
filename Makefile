Drun: build
	clear ; ./test_dynamic

Srun: build
	clear ; ./test_static

build: clean test_dynamic test_static

build_dynamic: clean test_dynamic

build_static: clean test_static

lbuild: libcguieng.so libcguieng.a

test_static: test.c libcguieng.a
	clang -g $^ -o $@

libcguieng.a: cguieng.o
	ar -rcv $@ $^

test_dynamic: test.c libcguieng.so
	clang test.c -L. -lcguieng -o $@

libcguieng.so: cguieng.o
	clang -shared -o $@ $^

cguieng.o: cguieng.h cguieng.c
	clang -c -fPIC cguieng.c -o $@

test: test.c
	clang -g test.c -o $@

set_env:
	LD_LIBRARY_PATH=../ConsoleGUI ; export LD_LIBRARY_PATH

clean:
	rm -f *.a *.so *.o test_* test