/**
 * hand_index.h
 *
 * @author Kevin Waugh (waugh@cs.cmu.edu)
 * @date April 13, 2013
 * 
 * map poker hands to an index shared by all isomorphic hands, and
 * map an index to a canonical poker hand
 */

#ifndef _HAND_INDEX_H_
#define _HAND_INDEX_H_

#include <inttypes.h>
#include <stddef.h>
#include "deck.h"

#define MAX_ROUNDS           8

typedef uint64_t hand_index_t;
typedef struct hand_indexer_s hand_indexer_t;
typedef struct hand_indexer_state_s hand_indexer_state_t;

#define PRIhand_index        PRIu64

/**
 * @param rounds number of rounds
 * @param cards_per_round number of cards in each round
 * @param indexer 
 */
_Bool hand_indexer_init(uint_fast32_t rounds, const uint8_t cards_per_round[], hand_indexer_t * indexer);

/**
 * @param indexer
 */
void hand_indexer_free(hand_indexer_t * indexer);

/**
 * @param indexer
 * @param round
 * @returns size of index up to and including round
 */
hand_index_t hand_indexer_size(const hand_indexer_t * indexer, uint_fast32_t round);

void hand_indexer_state_init(const hand_indexer_t * indexer, hand_indexer_state_t * state);

hand_index_t hand_index_all(const hand_indexer_t * indexer, const uint8_t cards[], hand_index_t indices[]);

hand_index_t hand_index_last(const hand_indexer_t * indexer, const uint8_t cards[]);

hand_index_t hand_index_next_round(const hand_indexer_t * indexer, const uint8_t cards[], hand_indexer_state_t * state);

_Bool hand_unindex(const hand_indexer_t * indexer, uint_fast32_t round, hand_index_t index, uint8_t cards[]);

#include "hand_index-impl.h"

#endif /* _HAND_INDEX_H_ */
