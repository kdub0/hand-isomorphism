/**
 * rank_set.h
 *
 * @author Kevin Waugh (waugh@cs.cmu.edu)
 * @date April 7, 2013
 *
 * index and unindex sets of ranks.
 */
#ifndef _RANK_SET_H_
#define _RANK_SET_H_

#include <assert.h>
#include <inttypes.h>
#include <stddef.h>
#include "deck.h"

typedef uint_fast16_t rank_set_t;

#define PRIrank_set             PRIuFAST16
#define SCNrank_set             SCNuFAST16

#define EMPTY_RANK_SET          0
#define INVALID_RANK_SET        UINT_FAST16_MAX
#define RANK_SETS               (1u<<RANKS)

typedef uint64_t rank_set_index_t;

#define PRIrank_set_index       PRIu64
#define SCNrank_set_index       SCNu64

#define INVALID_RANK_SET_INDEX  UINT64_MAX

/**
 * @param set rank set
 * @returns size of rank set
 */
size_t rank_set_size(rank_set_t set);

/**
 * @param n length of ranks
 * @param ranks rank array
 * @returns a rank set from an array of ranks, INVALID_RANK_SET if duplicates are detected
 */
rank_set_t rank_set_from_rank_array(size_t n, const card_t ranks[]);

/**
 * @param n length of cards
 * @param cards card array
 * @param suit suit to extract
 * @returns a rank set, INVALID_RANK_SET if duplicates are detected
 */
rank_set_t rank_set_from_card_array(size_t n, const card_t cards[], card_t suit);

/**
 * @param set set to unbox
 * @param ranks output
 * @returns true on success 
 */
_Bool rank_set_to_rank_array(rank_set_t set, card_t ranks[]);

/**
 * @param set set to unbox
 * @param cards output
 * @param suit suit of cards
 * @returns true on success 
 */
_Bool rank_set_to_card_array(rank_set_t set, card_t cards[], card_t suit);

/**
 * @param m size of set to be indexed
 * @param used ranks that are unavailable
 * @returns number of sets of size m
 */
rank_set_index_t rank_set_index_size(size_t m, rank_set_t used);

/**
 * @param m size of the indexed set
 * @param index index of rank set
 * @param used ranks that are unavailable
 * @returns true if index is valid
 */
_Bool rank_set_index_valid(size_t m, rank_set_index_t index, rank_set_t used);

/**
 * @param set set to index
 * @returns index of rank set between [0,rank_set_index_size(rank_set_size(set),0)), or
 * INVALID_RANK_SET_INDEX if not possible
 * @note uses combinatorics, not tabulated
 * @note specialization of rank_set_index_nCr
 */
rank_set_index_t rank_set_index_nCr_empty(rank_set_t set); 
    
/**
 * @param set set to index
 * @param used ranks that are unavailable
 * @returns index of rank set between [0,rank_set_index_size(rank_set_size(set),used)), or
 * INVALID_RANK_SET_INDEX if not possible
 * @note uses combinatorics, not tabulated
 */
rank_set_index_t rank_set_index_nCr(rank_set_t set, rank_set_t used);

/**
 * @param m size of set to be unindexed
 * @param index index of rank set 
 * @param used ranks that are unavailable
 * @returns rank set represented by index, or INVALID_RANK_SET if index is invalid, or
 * m + rank_set_size(used) > RANKS
 * @note uses combinatorics, not tabulated
 */
rank_set_t rank_set_unindex_nCr(size_t m, rank_set_index_t index, rank_set_t used);

/**
 * @param set set to index
 * @returns index of rank set between [0,rank_set_index_size(rank_set_size(set),0)), or
 * INVALID_RANK_SET_INDEX if not possible
 * @note specialization of rank_set_index
 */
rank_set_index_t rank_set_index_empty(rank_set_t set); 

/**
 * @param set set to index
 * @param used ranks that are unavailable
 * @returns index of rank set between [0,rank_set_index_size(rank_set_size(set),used)), or
 * INVALID_RANK_SET_INDEX if not possible
 */
rank_set_index_t rank_set_index(rank_set_t set, rank_set_t used);

/**
 * @param m size of set to be unindexed
 * @param index index of rank set 
 * @param used ranks that are unavailable
 * @returns rank set represented by index, or INVALID_RANK_SET if index is invalid, or
 * m + rank_set_size(used) > RANKS
 */
rank_set_t rank_set_unindex(size_t m, rank_set_index_t index, rank_set_t used);

/**
 * @param set
 * @returns true if rank set is valid
 */
static inline _Bool rank_set_valid(rank_set_t set) {
  return set < RANK_SETS;
}

/**
 * @param set
 * @returns true if rank set is empty
 * @note set must be valid
 */
static inline _Bool rank_set_empty(rank_set_t set) {
  assert(rank_set_valid(set));
  return set == EMPTY_RANK_SET;
}

/**
 * @param set
 * @returns true if rank set contains a single rank
 * @note rank set must be valid
 */
static inline _Bool rank_set_is_singleton(rank_set_t set) {
  assert(rank_set_valid(set));
  return set && (set&(set-1)) == 0;
}

/**
 * @param rank
 * @returns rank set containing only rank
 * @note rank must be valid
 */
static inline rank_set_t rank_set_from_rank(card_t rank) {
  assert(deck_valid_rank(rank));
  return 1u<<rank;
}

/**
 * @param set containing only a single rank
 * @returns rank contained in set
 */
static inline card_t rank_from_rank_set(rank_set_t set) {
  /* from: http://graphics.stanford.edu/~seander/bithacks.html */
  assert(rank_set_is_singleton(set));
  return !!(set & 0xaaaa)    |
         !!(set & 0xcccc)<<1 |
	 !!(set & 0xf0f0)<<2 |
	 !!(set & 0xff00)<<3;
}

/**
 * @param a
 * @param b
 * @returns set containing the ranks in the intersection of a and b
 * @note set a and b must be valid
 */
static inline rank_set_t rank_set_intersect(rank_set_t a, rank_set_t b) {
  assert(rank_set_valid(a)); assert(rank_set_valid(b));
  return a & b;
}

/**
 * @param a
 * @param b
 * @returns set containing the ranks in a, but not in b
 * @note set a and b must be valid
 */
static inline rank_set_t rank_set_subtract(rank_set_t a, rank_set_t b) {
  assert(rank_set_valid(a)); assert(rank_set_valid(b));
  return a & ~b;
}

/**
 * @param a
 * @param b
 * @returns set containing the ranks in a or in b
 * @note set a and b must be valid
 */
static inline rank_set_t rank_set_union(rank_set_t a, rank_set_t b) {
  assert(rank_set_valid(a)); assert(rank_set_valid(b));
  return a | b;
}

/**
 * @param set
 * @param rank
 * @returns true if set contains rank
 * @note set and rank must be valid
 */
static inline _Bool rank_set_is_set(rank_set_t set, card_t rank) {
  return rank_set_intersect(set, rank_set_from_rank(rank));
}

/**
 * @param set
 * @param rank
 * @returns set with rank added
 * @note set and rank must be valid
 * @note rank must not be in set
 */
static inline rank_set_t rank_set_add(rank_set_t set, card_t rank) {
  assert(!rank_set_is_set(set, rank));
  return rank_set_union(set, rank_set_from_rank(rank));
}

/**
 * @param set
 * @param rank
 * @returns set without rank in it
 * @note rank must be in set
 */
static inline rank_set_t rank_set_remove(rank_set_t set, card_t rank) {
  assert(rank_set_is_set(set, rank));
  return rank_set_subtract(set, rank_set_from_rank(rank));
}

/**
 * @param set
 * @param rank
 * @returns set with rank in it
 * @note set and rank must be valid
 * @note rank may already be in set
 */
static inline rank_set_t rank_set_set(rank_set_t set, card_t rank) {
  return rank_set_union(set, rank_set_from_rank(rank));
}

/**
 * @param set
 * @param rank
 * @returns set without rank in it
 * @note set and rank must be valid
 * @note rank may already be in set
 */
static inline rank_set_t rank_set_unset(rank_set_t set, card_t rank) {
  return rank_set_subtract(set, rank_set_from_rank(rank));
}

/**
 * @param set
 * @param rank
 * @returns set where rank is added if its not already in the set, and 
 * removed otherwise
 * @note set and rank must be valid
 */
static inline rank_set_t rank_set_toggle(rank_set_t set, card_t rank) {
  assert(rank_set_valid(set));
  return set ^ rank_set_from_rank(rank);
}

/**
 * @param set
 * @returns smallest rank in the set
 * @note set must be valid and non-empty
 */
static inline card_t rank_set_first(rank_set_t set) {
  assert(!rank_set_empty(set));
  return rank_from_rank_set(set&~(set-1));
}

/**
 * removes smallest rank in the set and returns it
 * @param set
 * @returns smallest rank in the set
 * @note set must be valid and non-empty
 */
static inline card_t rank_set_next(rank_set_t * set) {
  rank_set_t first_set = (*set) & ~((*set) - 1);
  *set = rank_set_subtract(*set, first_set);
  return rank_from_rank_set(first_set);
}

#endif
