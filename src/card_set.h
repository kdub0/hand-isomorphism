/**
 * card_set.h
 *
 * @author Kevin Waugh (waugh@cs.cmu.edu)
 * @date April 7, 2013
 *
 * index and unindex sets of cards.
 */
#ifndef _CARD_SET_H_
#define _CARD_SET_H_

#include <assert.h>
#include <inttypes.h>
#include <stddef.h>
#include "deck.h"

typedef uint_fast16_t card_set_t;

#define PRIcard_set             PRIuFAST16
#define SCNcard_set             SCNuFAST16

#define EMPTY_CARD_SET          0
#define INVALID_CARD_SET        UINT_FAST16_MAX

typedef uint64_t card_set_index_t;

#define PRIcard_set_index       PRIu64
#define SCNcard_set_index       SCNu64

#define INVALID_CARD_SET_INDEX  UINT64_MAX

/**
 * @param set card set
 * @returns size of card set
 */
size_t card_set_size(card_set_t set);

/**
 * @param n length of ranks
 * @param ranks rank array
 * @returns a card set from an array of ranks, INVALID_CARD_SET if duplicates are detected
 */
card_set_t card_set_from_rank_array(size_t n, const card_t ranks[]);

/**
 * @param n length of cards
 * @param cards card array
 * @param suit suit to extract
 * @returns a card set, INVALID_CARD_SET if duplicates are detected
 */
card_set_t card_set_from_card_array(size_t n, const card_t cards[], card_t suit);

/**
 * @param set set to unbox
 * @param ranks output
 * @returns true on success 
 */
_Bool card_set_to_rank_array(card_set_t set, card_t ranks[]);

/**
 * @param set set to unbox
 * @param cards output
 * @param suit suit of cards
 * @returns true on success 
 */
_Bool card_set_to_card_array(card_set_t set, card_t cards[], card_t suit);

/**
 * @param m size of set to be indexed
 * @param used cards that are unavailable
 * @returns number of sets of size m
 */
card_set_index_t card_set_index_size(size_t m, card_set_t used);

/**
 * @param m size of the indexed set
 * @param index index of card set
 * @param used cards that are unavailable
 * @returns true if index is valid
 */
_Bool card_set_index_valid(size_t m, card_set_index_t index, card_set_t used);

/**
 * @param set set to index
 * @param used cards that are unavailable
 * @returns index of card set between [0,card_set_index_size(card_set_size(set),used)), or
 * INVALID_CARD_SET_INDEX if not possible
 */
card_set_index_t card_set_index(card_set_t set, card_set_t used);

/**
 * @param m size of set to be unindexed
 * @param index index of card set 
 * @param used cards that are unavailable
 * @returns card set represented by index, or INVALID_CARD_SET if index is invalid, or
 * m + card_set_size(used) > RANKS
 */
card_set_t card_set_unindex(size_t m, card_set_index_t index, card_set_t used);

/**
 * @param set
 * @returns true if card set is valid
 */
static inline _Bool card_set_valid(card_set_t set) {
  return set != INVALID_CARD_SET;
}

/**
 * @param set
 * @returns true if card set is empty
 * @note set must be valid
 */
static inline _Bool card_set_empty(card_set_t set) {
  assert(card_set_valid(set));
  return set == EMPTY_CARD_SET;
}

/**
 * @param set
 * @returns true if card set contains a single card
 * @note card set must be valid
 */
static inline _Bool card_set_is_singleton(card_set_t set) {
  assert(card_set_valid(set));
  return set && (set&(set-1)) == 0;
}

/**
 * @param card
 * @returns card set containing only card
 * @note card must be valid
 */
static inline card_set_t card_set_from_card(card_t card) {
  assert(deck_valid_card(card));
  return 1u<<card;
}

/**
 * @param set containing only a single card
 * @returns card contained in set
 */
static inline card_t card_from_card_set(card_set_t set) {
  /* from: http://graphics.stanford.edu/~seander/bithacks.html */
  assert(card_set_is_singleton(set));
  return !!(set & 0xaaaa)    |
         !!(set & 0xcccc)<<1 |
	 !!(set & 0xf0f0)<<2;
}

/**
 * @param a
 * @param b
 * @returns set containing the cards in the intersection of a and b
 * @note set a and b must be valid
 */
static inline card_set_t card_set_intersect(card_set_t a, card_set_t b) {
  assert(card_set_valid(a)); assert(card_set_valid(b));
  return a & b;
}

/**
 * @param a
 * @param b
 * @returns set containing the cards in a, but not in b
 * @note set a and b must be valid
 */
static inline card_set_t card_set_subtract(card_set_t a, card_set_t b) {
  assert(card_set_valid(a)); assert(card_set_valid(b));
  return a & ~b;
}

/**
 * @param a
 * @param b
 * @returns set containing the cards in a or in b
 * @note set a and b must be valid
 */
static inline card_set_t card_set_union(card_set_t a, card_set_t b) {
  assert(card_set_valid(a)); assert(card_set_valid(b));
  return a | b;
}

/**
 * @param set
 * @param card
 * @returns true if set contains card
 * @note set and card must be valid
 */
static inline _Bool card_set_is_set(card_set_t set, card_t card) {
  return card_set_intersect(set, card_set_from_card(card));
}

/**
 * @param set
 * @param card
 * @returns set with card added
 * @note set and card must be valid
 * @note card must not be in set
 */
static inline card_set_t card_set_add(card_set_t set, card_t card) {
  assert(!card_set_is_set(set, card));
  return card_set_union(set, card_set_from_card(card));
}

/**
 * @param set
 * @param card
 * @returns set without card in it
 * @note card must be in set
 */
static inline card_set_t card_set_remove(card_set_t set, card_t card) {
  assert(card_set_is_set(set, card));
  return card_set_subtract(set, card_set_from_card(card));
}

/**
 * @param set
 * @param card
 * @returns set with card in it
 * @note set and card must be valid
 * @note card may already be in set
 */
static inline card_set_t card_set_set(card_set_t set, card_t card) {
  return card_set_union(set, card_set_from_card(card));
}

/**
 * @param set
 * @param card
 * @returns set without card in it
 * @note set and card must be valid
 * @note card may already be in set
 */
static inline card_set_t card_set_unset(card_set_t set, card_t card) {
  return card_set_subtract(set, card_set_from_card(card));
}

/**
 * @param set
 * @param card
 * @returns set where card is added if its not already in the set, and 
 * removed otherwise
 * @note set and card must be valid
 */
static inline card_set_t card_set_toggle(card_set_t set, card_t card) {
  assert(card_set_valid(set));
  return set ^ card_set_from_card(card);
}

/**
 * @param set
 * @returns smallest card in the set
 * @note set must be valid and non-empty
 */
static inline card_t card_set_first(card_set_t set) {
  assert(!card_set_empty(set));
  return card_from_card_set(set&(set-1));
}

/**
 * removes smallest card in the set and returns it
 * @param set
 * @returns smallest card in the set
 * @note set must be valid and non-empty
 */
static inline card_t card_set_next(card_set_t * set) {
  card_set_t first_set = (*set) & ((*set) - 1);
  *set = card_set_subtract(*set, first_set);
  return card_from_card_set(first_set);
}

#endif
