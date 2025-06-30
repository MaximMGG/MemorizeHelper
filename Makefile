NAME = memorize
CC = gcc
GLAGS = -Wall
LIBS = -lcext

SRC_DIR = ./src
OBJ_DIR = ./obj

SRC = $(wildcard $(SRC_DIR)/*.c)

OBJ = $(SRC:*.c=$(OBJ_DIR)/*.o)


.PHONY = build clean run


default: build run

run:
	./$(NAME)

build: $(OBJ)
	$(CC) -o $(NAME) $^ $(FALGS) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $< -o $@ $(FALGS) $(LIBS)
	

clear:
	rm $(NAME) $(OBJ_DIR)/*.o


