GCC=gcc
SOURCE_SERVER=server
SOURCE_CLIENT=client
FLAGS=-Wall

all: compile
	./$(SOURCE_SERVER)

compile:
	$(GCC) $(SOURCE_SERVER).c -o $(SOURCE_SERVER) $(FLAGS)
	$(GCC) $(SOURCE_CLIENT).c -o $(SOURCE_CLIENT) $(FLAGS)

clear:
	rm -rf $(SOURCE_SERVER) $(SOURCE_CLIENT)