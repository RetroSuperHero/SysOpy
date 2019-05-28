CC = gcc
SERVER=server
CLIENT=client
FLAGS=-Wall -lpthread -lm

all: compile
	./$(SERVER)

compile:
	$(CC) $(SERVER).c -o $(SERVER) $(FLAGS)
	$(CC) $(CLIENT).c -o $(CLIENT) $(FLAGS)

clean:
	rm -rf $(SERVER) $(CLIENT)