fazell: ./src/fazell.c
	+@[ -d $(./bin $@) ] || mkdir -p $(./bin $@)
	gcc -o ./bin/fazell ./src/fazell.c

install:
	mv ./bin/fazell ~/bin

