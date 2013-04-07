#
# Makefile
# author: Kevin Waugh (waugh@cs.cmu.edu)
# date: April 7, 2013
#
# see README.md and LICENSE.txt for more details.
#

CFLAGS   ?=-std=c99 -Wall -g -O2

SRC      :=$(addprefix src/,card_set.c)
OBJ      :=$(SRC:.c=.o)

TEST_SRC :=$(addprefix src/,test.c)
TEST_OBJ :=$(TEST_SRC:.c=.o)

.PHONY: all clean check

all: index_flop unindex_flop

clean:
	rm -f src/check index_flop unindex_flop src/check-main.o src/index_flop-main.o src/unindex_flop-main.o
	rm -f $(TEST_OBJ) $(OBJ)

check: src/check
	./src/check

src/check: src/check-main.o $(TEST_OBJ) $(OBJ)

index_flop: src/index_flop-main.o $(OBJ)

unindex_flop: src/unindex_flop-main.o $(OBJ)

src/check index_flop unindex_flop:
	$(CC) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

src/card_set.o: src/card_set.h src/deck.h
src/check-main.c: src/test.h

$(TEST_SRC): src/test.h
