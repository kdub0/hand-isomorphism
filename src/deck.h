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

#include <assert.h>
#include <inttypes.h>

/*
 * REQUIRE: RANKS < 16
 * REQUIRE: CARDS <= 64
 */
#define SUITS                4
#define RANKS                13
#define CARDS                52
#define MAX_ROUNDS           8

typedef uint_fast8_t card_t;

#define PRIcard              PRIuFAST8
#define SCNcard              SCNuFAST8

/**
 * @param card
 * @returns true if card is in the valid range
 */
static inline _Bool deck_valid_card(card_t card) {
  return card < CARDS;
}

/**
 * @param suit
 * @returns true if suit is in the valid range
 */
static inline _Bool deck_valid_suit(card_t suit) {
  return suit < SUITS;
}

/**
 * @param rank
 * @returns true if rank is in the valid range
 */
static inline _Bool deck_valid_rank(card_t rank) {
  return rank < RANKS;
}

/**
 * @param suit
 * @param rank
 * @returns specified card
 * @note suit and rank must be valid
 */
static inline card_t deck_make_card(card_t suit, card_t rank) {
  assert(deck_valid_suit(suit)); assert(deck_valid_rank(rank));
  return rank<<2 | suit;
}

/**
 * @param card
 * @returns card's suit
 * @note card must be valid
 */
static inline card_t deck_get_suit(card_t card) {
  assert(deck_valid_card(card));
  return card&3;
}

/**
 * @param card
 * @returns card's rank
 * @note rank must be valid
 */
static inline card_t deck_get_rank(card_t card) {
  assert(deck_valid_card(card));
  return card>>2;
}

#endif
