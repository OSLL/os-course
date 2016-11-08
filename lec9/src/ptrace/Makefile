CC ?= cc
SRC := $(shell find . -name '*.c')
OBJ := $(SRC:.c=.o)
DEP := $(SRC:.c=.d)

all: trace

trace: $(OBJ)
	$(CC) $^ -o $@

$(OBJ): %.o: %.c
	$(CC) -MMD -c $< -o $@

-include $(DEP)

.PHONY: clean
clean:
	rm -rf trace $(OBJ) $(DEP)
