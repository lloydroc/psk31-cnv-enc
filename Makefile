P=psk31_enc
OBJECTS=
CFLAGS=-g -Wall -O3
LDLIBS=
CC=gcc

$(P): $(OBJECTS)

clean:
	rm $(P)
	rm -rf $(P).dSYM
