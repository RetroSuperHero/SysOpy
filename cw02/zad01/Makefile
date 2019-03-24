CC = gcc
SOURCE = main
CFLAGS = -Wall -O
OUTPUT = wyniki.txt
SMALLNUMBER = 1000
BIGNUMBER = 5000
SIZE1 = generate tmp $(SMALLNUMBER) 1 copy tmp tmp2 $(SMALLNUMBER) 1 sys copy tmp tmp3 $(SMALLNUMBER) 1 lib sort tmp2 $(SMALLNUMBER) 1 sys sort tmp3 $(SMALLNUMBER) 1 lib generate tmp $(BIGNUMBER) 1 copy tmp tmp2 $(BIGNUMBER) 1 sys copy tmp tmp3 $(BIGNUMBER) 1 lib sort tmp2 $(BIGNUMBER) 1 sys sort tmp3 $(BIGNUMBER) 1 lib
SIZE4 = generate tmp $(SMALLNUMBER) 4 copy tmp tmp2 $(SMALLNUMBER) 4 sys copy tmp tmp3 $(SMALLNUMBER) 4 lib sort tmp2 $(SMALLNUMBER) 4 sys sort tmp3 $(SMALLNUMBER) 4 lib generate tmp $(BIGNUMBER) 4 copy tmp tmp2 $(BIGNUMBER) 4 sys copy tmp tmp3 $(BIGNUMBER) 4 lib sort tmp2 $(BIGNUMBER) 4 sys sort tmp3 $(BIGNUMBER) 4 lib
SIZE512 = generate tmp $(SMALLNUMBER) 512 copy tmp tmp2 $(SMALLNUMBER) 512 sys copy tmp tmp3 $(SMALLNUMBER) 512 lib sort tmp2 $(SMALLNUMBER) 512 sys sort tmp3 $(SMALLNUMBER) 512 lib generate tmp $(BIGNUMBER) 512 copy tmp tmp2 $(BIGNUMBER) 512 sys copy tmp tmp3 $(BIGNUMBER) 512 lib sort tmp2 $(BIGNUMBER) 512 sys sort tmp3 $(BIGNUMBER) 512 lib
SIZE1024 = generate tmp $(SMALLNUMBER) 1024 copy tmp tmp2 $(SMALLNUMBER) 1024 sys copy tmp tmp3 $(SMALLNUMBER) 1024 lib sort tmp2 $(SMALLNUMBER) 1024 sys sort tmp3 $(SMALLNUMBER) 1024 lib generate tmp $(BIGNUMBER) 1024 copy tmp tmp2 $(BIGNUMBER) 1024 sys copy tmp tmp3 $(BIGNUMBER) 1024 lib sort tmp2 $(BIGNUMBER) 1024 sys sort tmp3 $(BIGNUMBER) 1024 lib
SIZE4096 = generate tmp $(SMALLNUMBER) 4096 copy tmp tmp2 $(SMALLNUMBER) 4096 sys copy tmp tmp3 $(SMALLNUMBER) 4096 lib sort tmp2 $(SMALLNUMBER) 4096 sys sort tmp3 $(SMALLNUMBER) 4096 lib generate tmp $(BIGNUMBER) 4096 copy tmp tmp2 $(BIGNUMBER) 4096 sys copy tmp tmp3 $(BIGNUMBER) 4096 lib sort tmp2 $(BIGNUMBER) 4096 sys sort tmp3 $(BIGNUMBER) 4096 lib
SIZE8192 = generate tmp $(SMALLNUMBER) 8192 copy tmp tmp2 $(SMALLNUMBER) 8192 sys copy tmp tmp3 $(SMALLNUMBER) 8192 lib sort tmp2 $(SMALLNUMBER) 8192 sys sort tmp3 $(SMALLNUMBER) 8192 lib generate tmp $(BIGNUMBER) 8192 copy tmp tmp2 $(BIGNUMBER) 8192 sys copy tmp tmp3 $(BIGNUMBER) 8192 lib sort tmp2 $(BIGNUMBER) 8192 sys sort tmp3 $(BIGNUMBER) 8192 lib

compile:
	$(CC) $(SOURCE).c -o $(SOURCE)

1: 
	./$(SOURCE) $(SIZE1) >> $(OUTPUT) 

4:
	./$(SOURCE) $(SIZE4) >> $(OUTPUT) 

512:
	./$(SOURCE) $(SIZE512) >> $(OUTPUT) 

1024:
	./$(SOURCE) $(SIZE1024) >> $(OUTPUT) 

4096:
	./$(SOURCE) $(SIZE4096) >> $(OUTPUT) 

8192:
	./$(SOURCE) $(SIZE8192) >> $(OUTPUT) 


all: 
	make compile
	make 1
	make 4
	make 512
	make 1024
	make 4096
	make 8192

clean:
	rm -r $(SOURCE) $(OUTPUT)
