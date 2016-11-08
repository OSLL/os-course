CC ?= cc
SRC := $(shell find . -name '*.c')
OBJ := $(SRC:.c=.o)
DEP := $(SRC:.c=.d)

all: shmem

shmem: $(OBJ)
	$(CC) $^ -lrt -o $@

$(OBJ): %.o: %.c
	$(CC) -MMD -c $< -o $@

-include $(DEP)

.PHONY: clean
clean:
	rm -rf shmem $(OBJ) $(DEP)
