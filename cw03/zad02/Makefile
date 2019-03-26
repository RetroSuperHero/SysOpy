GCC=gcc
SOURCE=main
FLAGS=-Wall
PARAMETERS=lista 30 0
ARCHIVE=archive

all:
	make compile
	./$(SOURCE) $(PARAMETERS)

compile:
	$(GCC) $(SOURCE).c -o $(SOURCE) $(FLAGS)

clear:
	rm -rf $(ARCHIVE) $(SOURCE)
	mkdir $(ARCHIVE)