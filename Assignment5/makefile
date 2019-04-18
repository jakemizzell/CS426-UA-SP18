all : fifo lru

fifo: fifo.c
	gcc -Wall -std=c11 fifo.c -o fifo

lru: lru.c
	gcc -Wall -std=c11 lru.c -o lru

test : fifo lru
	./fifo addresses.txt
	./lru addresses.txt

clean   :
	rm -f fifo lru