CC ?= cc

all: hello

hello: hello.S
	$(CC) $< -o $@

.PHONY: clean
clean:
	rm -rf hello
