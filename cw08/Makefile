GCC=gcc
SOURCE=main
PARAMETERS1=1 block inFile.pgm filter.txt outFile.pgm
PARAMETERS2=2 block inFile.pgm filter.txt outFile.pgm
PARAMETERS3=4 block inFile.pgm filter.txt outFile.pgm
PARAMETERS4=8 block inFile.pgm filter.txt outFile.pgm
PARAMETERS5=20 block inFile.pgm filter.txt outFile.pgm
FLAGS=-Wall -lpthread -lm

all: compile
	./$(SOURCE) $(PARAMETERS1)
	./$(SOURCE) $(PARAMETERS2)
	./$(SOURCE) $(PARAMETERS3)
	./$(SOURCE) $(PARAMETERS4)
	./$(SOURCE) $(PARAMETERS5)

compile:
	$(GCC) $(SOURCE).c -o $(SOURCE) $(FLAGS)

clean:
	rm -rf $(SOURCE)