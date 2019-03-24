CC = gcc
SOURCE=main
CFLAGS=-Wall -O
LIBPATH=../zad01
LIBNAME=library
OUTPUT=raport2.txt
PARAMETERS=create_table 20 search_directory . main tmp1 search_directory ../zad01 main tmp2 remove_block 1 search_directory ../zad01 library.h tmp3 search_directory . aa tmp4 search_directory ../../../../Geo Geo tmp5

all: 
	$(CC) -I$(LIBPATH) $(SOURCE).c $(LIBPATH)/$(LIBNAME).c -o $(SOURCE)
	echo 'Statystyki:\n' >> $(OUTPUT)
	echo 'Parametry:\n$(PARAMETERS)\n' >> $(OUTPUT)
	./$(SOURCE) $(PARAMETERS) >> $(OUTPUT) 

clean:
	rm -r $(SOURCE) $(OUTPUT)
