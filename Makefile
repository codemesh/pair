CC=gcc
CFLAGS=-g
pair : pair.c
	$(CC) $(CFLAGS) -o $@ $^