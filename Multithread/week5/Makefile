CC = clang
CC_FLAGS = -std=c11
SRC = unnamed_pipes.c
EXEC = unnamed_pipes

.PHONY: all

all: $(EXEC)

.c:
	@$(CC) -o $@ $< $(CC_FLAGS)
