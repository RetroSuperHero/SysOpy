GCC=gcc
SOURCE=main
PARAMETERS1=1 block inFile.pgm filter.txt outFile.pgm
PARAMETERS2=2 block inFile.pgm filter.txt outFile.pgm
PARAMETERS3=4 block inFile.pgm filter.txt outFile.pgm
PARAMETERS4=8 block inFile.pgm filter.txt outFile.pgm
PARAMETERS5=1 interleaved inFile.pgm filter.txt outFile.pgm
PARAMETERS6=2 interleaved inFile.pgm filter.txt outFile.pgm
PARAMETERS7=4 interleaved inFile.pgm filter.txt outFile.pgm
PARAMETERS8=8 interleaved inFile.pgm filter.txt outFile.pgm
TIMES=Times.txt
FLAGS=-Wall -lpthread -lm

all: compile
	./$(SOURCE) $(PARAMETERS1) >> $(TIMES)
	./$(SOURCE) $(PARAMETERS2) >> $(TIMES)
	./$(SOURCE) $(PARAMETERS3) >> $(TIMES)
	./$(SOURCE) $(PARAMETERS4) >> $(TIMES)
	./$(SOURCE) $(PARAMETERS5) >> $(TIMES)
	./$(SOURCE) $(PARAMETERS6) >> $(TIMES)
	./$(SOURCE) $(PARAMETERS7) >> $(TIMES)
	./$(SOURCE) $(PARAMETERS8) >> $(TIMES)

compile:
	$(GCC) $(SOURCE).c -o $(SOURCE) $(FLAGS)

clean:
	rm -rf $(SOURCE) $(TIMES)