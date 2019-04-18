all : p536 pp3

p536: p536.c
	gcc -Wall -std=c11 p536.c -o p536 -lpthread

pp3: pp3.c
	gcc -Wall -std=c11 pp3.c -o pp3 -lpthread

test : p536 pp3
	./p536
	./pp3 5 5 5

clean   :
	rm -f p536 pp3

