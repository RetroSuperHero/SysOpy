GCC=gcc
SOURCE_SERVER=server
SOURCE_CLIENT=client
FLAGS=-Wall -lrt

all: compile
	./$(SOURCE_SERVER)

compile:
	$(GCC) $(SOURCE_SERVER).c -o $(SOURCE_SERVER) $(FLAGS)
	$(GCC) $(SOURCE_CLIENT).c -o $(SOURCE_CLIENT) $(FLAGS)

clean:
	rm -rf $(SOURCE_SERVER) $(SOURCE_CLIENT)