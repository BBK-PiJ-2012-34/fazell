fazell: ./src/fazell.c
	mkdir ./bin
	gcc -o ./bin/fazell ./src/fazell.c

install:
	mv ./bin/fazell ~/bin

