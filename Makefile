OBJS = graphs.c

CC = clang

COMPILER_FLAGS = -w

LINKER_FLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

OBJ_NAME = main

all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
