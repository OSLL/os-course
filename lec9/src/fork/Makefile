CC ?= cc
SRC := $(shell find . -name '*.c')
OBJ := $(SRC:.c=.o)
DEP := $(SRC:.c=.d)

all: fork

fork: $(OBJ)
	$(CC) $^ -o $@

$(OBJ): %.o: %.c
	$(CC) -MMD -c $< -o $@

-include $(DEP)

.PHONY: clean
clean:
	rm -rf fork $(OBJ) $(DEP)
