GCC=gcc
MASTER=master
SLAVE=slave
FLAGS=-Wall
FIFO=fifo

all: compile
	./$(MASTER) $(FIFO)

compile:
	$(GCC) $(MASTER).c -o $(MASTER) $(FLAGS)
	$(GCC) $(SLAVE).c -o $(SLAVE) $(FLAGS)

clear:
	rm -rf $(MASTER) $(SLAVE) $(FIFO)