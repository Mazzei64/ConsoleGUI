# ConsoleGUI
Engine project for developing GUI-like and game console application.

## Makefile

Makefile suggested for compiling and testing the project:
```Makefile
run: build
	clear ; ./test_static

build: test_dynamic test_static clean

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
	cc -g test.c -o $@

clean:
	rm -f *.a *.so *.o test_*
```
