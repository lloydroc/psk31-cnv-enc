all: psk31_enc.o ex1.o ex_psk31.o

%.o: %.c
	cc -o $@ $<

clean:
	rm *.o
	rm *.txt
