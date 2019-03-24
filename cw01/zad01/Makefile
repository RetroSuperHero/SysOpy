CC = gcc
CFLAGS = -O -Wall
LIB = library

both:
	make static
	make shared

static:

	$(CC) -c $(LIB).c -o $(LIB).o
	ar cr $(LIB).a $(LIB).o

	make clean

shared:

	$(CC) -fPIC -c $(LIB).c -o $(LIB).o
	$(CC) -fPIC -shared -o lib_$(LIB).so $(LIB).o

	make clean

clean:
	rm -f *.o