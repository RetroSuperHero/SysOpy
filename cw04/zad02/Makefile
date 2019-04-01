GCC=gcc
SOURCE=main
FLAGS=-Wall
PARAMETERS=lista
ARCHIVE=archive

all:
	make compile
	./$(SOURCE) $(PARAMETERS)

compile:
	$(GCC) $(SOURCE).c -o $(SOURCE) $(FLAGS)

clear:
	rm -rf $(ARCHIVE) $(SOURCE)
	mkdir $(ARCHIVE)