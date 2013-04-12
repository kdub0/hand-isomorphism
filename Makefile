#
# Makefile
# author: Kevin Waugh (waugh@cs.cmu.edu)
# date: April 7, 2013
#
# see README.md and LICENSE.txt for more details.
#

CFLAGS   ?=-std=c99 -Wall -g -O2

SRC      :=$(addprefix src/,group_index.c rank_set.c)
OBJ      :=$(SRC:.c=.o)

TEST_SRC :=$(addprefix src/,group_index-test.c rank_set-test.c test.c)
TEST_OBJ :=$(TEST_SRC:.c=.o)

MAINS    :=check index_flop unindex_flop
MAIN_SRC :=$(addprefix src/,$(MAINS:=-main.c))
MAIN_OBJ :=$(MAIN_SRC:.c=.o)

BUILD    :=index_flop unindex_flop
TARGETS  :=src/check $(BUILD)

.PHONY: all clean check

all: $(BUILD)

clean:
	rm -f $(TARGETS)
	rm -f $(MAIN_OBJ) $(TEST_OBJ) $(OBJ)

check: src/check
	./src/check

src/check: src/check-main.o $(TEST_OBJ) $(OBJ)

$(BUILD): %: src/%-main.o $(OBJ)

$(TARGETS):
	$(CC) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

src/group_index.o: src/rank_set.h
src/group_index.o src/group_index-test.o: src/group_index.h src/deck.h
src/rank_set.o src/rank_set-test.o: src/rank_set.h src/deck.h
src/check-main.c: src/test.h

$(TEST_SRC): src/test.h
