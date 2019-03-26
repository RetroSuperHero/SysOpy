GCC=gcc
SOURCE=main
FLAGS=-Wall
PARAMETERS=.

all:
	make compile
	./$(SOURCE) $(PARAMETERS)

compile:
	$(GCC) $(SOURCE).c -o $(SOURCE) $(FLAGS)

clear:
	rm -rf $(SOURCE)