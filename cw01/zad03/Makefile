CC = gcc
SOURCE=main
CFLAGS=-Wall -Os
SRCPATH=../zad02
LIBPATH=../zad01
LIBNAME=library
OUTPUT=results3a.txt
PARAMETERS=create_table 5 search_directory . main tmp1 search_directory ../zad01 main tmp2 remove_block 1 search_directory ../zad01 library.h tmp3 search_directory . aa tmp4

shared_ver:
	$(CC) -I$(LIBPATH) $(SRCPATH)/$(SOURCE).c -o $(SOURCE)_shared_ver -L$(LIBPATH) $(LIBPATH)/$(LIBNAME).c -Wl,-rpath,$(LIBPATH)
	echo "\nOpcje GCC: $(CFLAGS), SHARED_LIB_STATS: \n" >> $(OUTPUT)
	./$(SOURCE)_shared_ver $(PARAMETERS) >> $(OUTPUT)

static_ver:
	$(CC) -static -I$(LIBPATH) $(SRCPATH)/$(SOURCE).c -o $(SOURCE)_static_ver -L$(LIBPATH) $(LIBPATH)/$(LIBNAME).c
	echo "\nOpcje GCC: $(CFLAGS), STATIC_LIB_STATS: \n" >> $(OUTPUT)
	./$(SOURCE)_static_ver $(PARAMETERS) >> $(OUTPUT)

dynamic_ver:
	$(CC) -I$(LIBPATH) $(SRCPATH)/$(SOURCE).c -o dyn_$(SOURCE) -L$(LIBPATH) $(LIBPATH)/$(LIBNAME).c -ldl
	echo "\nOpcje GCC: $(CFLAGS), DYNAMIC_LOADING__STATS: \n" >> $(OUTPUT)
	./dyn_$(SOURCE) $(PARAMETERS) >> $(OUTPUT)

all:
	make dynamic_ver
	make static_ver
	make shared_ver

clean:
	rm -r $(SOURCE)_shared_ver $(SOURCE)_static_ver dyn_$(SOURCE) $(OUTPUT)
