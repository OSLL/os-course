CC ?= cc

all: hello libfoo.so call_foo

hello: hello.S
	$(CC) $< -o $@

libfoo.so: foo.c foo.h
	$(CC) -fpic -shared -Wl,-soname,libfoo.so $< -o $@

call_foo: call_foo.c foo.h
	$(CC) $< -L. -lfoo -o $@

.PHONY: clean
clean:
	rm -rf hello libfoo.so call_foo
