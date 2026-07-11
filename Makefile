NAME = memorize
CC = gcc
FLAGS = -Wall
LIBS = -lcext -lpq 

SRC_DIR = ./src
OBJ_DIR = ./obj

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: build clear run

default: build run

run:
	./$(NAME)

build: $(OBJ)
	$(CC) -o $(NAME) $^ $(FALGS) $(LIBS) -g

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $< -o $@ $(FALGS) $(LIBS)

clear:
	rm $(NAME) $(OBJ_DIR)/*.o


