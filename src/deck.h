/**
 * deck.h
 *
 * @author Kevin Waugh (waugh@cs.cmu.edu)
 * @date April 7, 2013
 *
 * utilities for a standard deck of cards.
 */
#ifndef _DECK_H_
#define _DECK_H_

#include <inttypes.h>

#define SUITS                4
#define RANKS                13
#define CARDS                52

typedef uint_fast8_t card_t;

#define PRIcard              PRIuFAST8
#define SCNcard              SCNuFAST8

/**
 * @returns true if card is in the valid range
 */
static inline _Bool deck_valid_card(card_t card) {
  return card < CARDS;
}

/**
 * @returns card's suit
 */
static inline card_t deck_get_suit(card_t card) {
  return card&3;
}

/**
 * @returns card's rank
 */
static inline card_t deck_get_rank(card_t card) {
  return card>>2;
}

#endif
