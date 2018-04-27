INC=/usr/local/ssl/include/
LIB=/usr/local/ssl/lib/

all: 
	gcc -L$(LIB) -o hash_break hash_break.c strmap.o -lcrypto -ldl

strmap.o: strmap.c strmap.h
	gcc -c strmap.c -o strmap.o 

clean:
	rm *.o
	rm hash_break

