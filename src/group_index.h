/**
 * group_index.h
 *
 * @author: Kevin Waugh (waugh@cs.cmu.edu)
 * @date: April 11, 2013
 */
#ifndef _GROUP_INDEX_H_
#define _GROUP_INDEX_H_

#include <inttypes.h>
#include <stddef.h>
#include "deck.h"

typedef uint64_t group_index_t;

#define PRIgroup_index       PRIu64
#define SCNgroup_index       SCNu64

#define INVALID_GROUP_INDEX  UINT64_MAX

/**
 * @param m number of groups
 * @param group_size size of each group
 * @returns true if the groups are valid
 */
_Bool group_index_valid(size_t m, const uint8_t group_size[]);

/**
 * @param m number of groups
 * @param group_size size of each group
 * @param cards array of cards of the same suit
 * @returns current index, or INVALID_GROUP_INDEX
 */
group_index_t group_index_size(size_t m, const uint8_t group_size[]);

/**
 * @param m number of groups
 * @param group_size size of each group
 * @param cards array of cards of the same suit
 * @returns current index, or INVALID_GROUP_INDEX
 */
group_index_t group_index_from_card_array(size_t m, const uint8_t group_size[], const card_t cards[]);

/**
 * @param m number of groups
 * @param group_size size of each group
 * @param ranks array of ranks
 * @returns current index, or INVALID_GROUP_INDEX
 */
group_index_t group_index_from_rank_array(size_t m, const uint8_t group_size[], const card_t ranks[]);

/**
 * @param m number of groups
 * @param group_size size of each group
 * @param index group index
 * @param suit suit of cards
 * @param cards output
 * @returns true if successful
 */
_Bool group_unindex_to_card_array(size_t m, const uint8_t group_size[], group_index_t index, card_t suit, card_t cards[]);

/**
 * @param m number of groups
 * @param group_size size of each group
 * @param index group index
 * @param ranks output
 * @returns true if successful
 */
_Bool group_unindex_to_rank_array(size_t m, const uint8_t group_size[], group_index_t index, card_t ranks[]);

#endif
