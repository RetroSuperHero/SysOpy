GCC=gcc
SOURCE1=main
SOURCE2=main2
FLAGS=-Wall

all:
	make compile
	./$(SOURCE1)
	./$(SOURCE2)

compile:
	$(GCC) $(SOURCE1).c -o $(SOURCE1) $(FLAGS)
	$(GCC) $(SOURCE2).c -o $(SOURCE2) $(FLAGS)

clear:
	rm -rf $(SOURCE1) $(SOURCE2)