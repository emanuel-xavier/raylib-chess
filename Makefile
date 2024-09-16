INCLUDES=./src/main.c ./src/game.c

build:
	cc $(INCLUDES) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o game

run: build
	./game

clean:
	rm -rf game
