fazell: ./src/fazell.c
	gcc -o ./bin/fazell ./src/fazell.c

install:
	mv ./bin/fazell ~/bin

