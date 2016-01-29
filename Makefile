CFLAGS = -std=gnu99 -g -Wall
LIBS   = -lncurses

test:
	gcc test.c $(CFLAGS) $(LIBS) -o test
