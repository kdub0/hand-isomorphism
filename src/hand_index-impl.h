/**
 * hand_index-impl.h
 * 
 * @author Kevin Waugh (waugh@cs.cmu.edu)
 * @date April 13, 2013
 */

#ifndef _HAND_INDEX_IMPL_H_
#define _HAND_INDEX_IMPL_H_

struct hand_indexer_s {
  uint8_t cards_per_round[MAX_ROUNDS], round_start[MAX_ROUNDS];
  uint_fast32_t rounds, configurations[MAX_ROUNDS], permutations[MAX_ROUNDS];
  hand_index_t round_size[MAX_ROUNDS];

  uint_fast32_t * permutation_to_configuration[MAX_ROUNDS], * permutation_to_pi[MAX_ROUNDS], * configuration_to_equal[MAX_ROUNDS];  
  uint_fast32_t (* configuration[MAX_ROUNDS])[SUITS];
  uint_fast32_t (* configuration_to_suit_size[MAX_ROUNDS])[SUITS];
  hand_index_t * configuration_to_offset[MAX_ROUNDS];
};

struct hand_indexer_state_s {
  uint_fast32_t suit_index[SUITS];
  uint_fast32_t suit_multiplier[SUITS];
  uint_fast32_t round, permutation_index, permutation_multiplier;
  uint32_t used_ranks[SUITS];
};

#endif /* _HAND_INDEX_IMPL_H_ */
