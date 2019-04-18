all : p420 p421

p420: p420.c
	gcc -Wall -std=c11 p420.c -o p420 -lpthread

p421: p421.c
	gcc -Wall -std=c11 p421.c -o p421 -lpthread

test : p420 p421
	./p420
	./p421

clean   :
	rm -f p420 p421
