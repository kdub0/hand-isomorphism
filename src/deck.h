/**
 * deck.h
 *
 * @author Kevin Waugh (waugh@cs.cmu.edu)
 * @date April 13, 2013
 *
 * utilities for manipulating a standard deck of cards
 */

#ifndef _DECK_H_
#define _DECK_H_

#include <inttypes.h>

#define SUITS     4
#define RANKS    13
#define CARDS    52

typedef uint_fast32_t card_t;

extern const char RANK_TO_CHAR[];
extern const char SUIT_TO_CHAR[];

static inline card_t deck_get_suit(card_t card) {
  return card&3;
}

static inline card_t deck_get_rank(card_t card) {
  return card>>2;
}

static inline card_t deck_make_card(card_t suit, card_t rank) {
  return rank<<2 | suit;
}

#endif /* _DECK_H_ */
