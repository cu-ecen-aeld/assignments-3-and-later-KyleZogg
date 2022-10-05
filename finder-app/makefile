all: writer

writer: writer.c
	gcc -o writer writer.c

clean: 
	rm -f ./writer

CROSS_COMPILE:
	aarch64-none-linux-gnu-gcc -o writer writer.c