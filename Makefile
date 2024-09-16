INCLUDES=./src/main.c ./src/game.c
SRC := $(wildcard *.c)   # All C source files
EXEC := my_program       # Output executable name

build:
	cc $(INCLUDES) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o game

run: build
	./game

clean:
	rm -rf game

watch:
	@while true; do \
		make run; \
	done

