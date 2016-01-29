CFLAGS = -std=gnu99 -g -Wall

test:
	gcc test.c $(CFLAGS) -o test
