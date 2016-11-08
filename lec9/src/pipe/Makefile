CC ?= cc
SRC := $(shell find . -name '*.c')
OBJ := $(SRC:.c=.o)
DEP := $(SRC:.c=.d)

all: pipe

pipe: $(OBJ)
	$(CC) $^ -o $@

$(OBJ): %.o: %.c
	$(CC) -MMD -c $< -o $@

-include $(DEP)

.PHONY: clean
clean:
	rm -rf pipe $(OBJ) $(DEP)
