CC=gcc
COPTS=-g -ansi -pedantic -Wall

ALL = turtle

all: $(ALL)

clean:
	-rm -rf *.o core *~

clobber: 
	-rm -rf *.o core *~ $(ALL) scanner

scanner: scanner.c
	$(CC) $(COPTS) -DTEST_SCANNER $^ -o $@

turtle: scanner.o symtab.o turtle.o interpreter.o
	$(CC) $(COPTS) $^ -o $@ -lm

.c.o:
	$(CC) -c $(COPTS) $<

scanner.o: scanner.c scanner.h
symtab.o: symtab.c symtab.h
interpreter.o: interpreter.c scanner.h symtab.h turtle.h
turtle.o: turtle.c turtle.h
