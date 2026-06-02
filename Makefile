CC=gcc
CFLAGS=-Wall -pthread -Iinclude
LDFLAGS=-lrt 

SRC = src/main.c src/tablero.c src/fichas.c
OUT=build/programa

all:
	mkdir -p build
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LDFLAGS)

run: all
	./$(OUT)

clean:
	rm -rf build