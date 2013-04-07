#
# Makefile
# author: Kevin Waugh (waugh@cs.cmu.edu)
# date: April 7, 2013
#
# see README.md and LICENSE.txt for more details.
#

CFLAGS ?=-std=c99 -Wall -g -O2

.PHONY: all clean check

all: index_flop unindex_flop

clean:
	rm -f src/check index_flop unindex_flop src/check-main.o src/index_flop-main.o src/unindex_flop-main.o

check: src/check
	./src/check

src/check: src/check-main.o

index_flop: src/index_flop-main.o

unindex_flop: src/unindex_flop-main.o

src/check index_flop unindex_flop:
	$(CC) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)
