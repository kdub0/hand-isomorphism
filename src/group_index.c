#include <stdbool.h>
#include "group_index.h"
#include "rank_set.h"

/**
 * @param m number of groups
 * @param group_size size of each group
 * @returns true if the groups are valid
 */
bool group_index_valid(size_t m, const uint8_t group_size[]) {
  size_t count = 0;
  for(int i=0; i<m; ++i) {
    count += group_size[i];
  }
  return count < RANKS;
}

group_index_t group_index_size(size_t m, const uint8_t group_size[]) {
  if (!group_index_valid(m, group_size)) {
    return false;
  }

  group_index_t size = 1;
  for(size_t i=0, count=0; i<m; count+=group_size[i++]) {
    rank_set_index_t this_size = rank_set_index_size_from_count(group_size[i], count);
    if (this_size == INVALID_RANK_SET_INDEX) {
      return INVALID_GROUP_INDEX;
    }
    size *= this_size;
  }

  return size;
}

group_index_t group_index_from_card_array(size_t m, const uint8_t group_size[], const card_t cards[]) {
  if (!group_index_valid(m, group_size)) {
    return false;
  }

  card_t ranks[RANKS], suit;
  for(int i=0, j=0; i<m; j+=group_size[i++]) {
    for(int k=0; k<group_size[i]; ++k) {
      if (!deck_valid_card(cards[j+k])) {
        return INVALID_GROUP_INDEX;
      }

      card_t this_suit = deck_get_suit(cards[j+k]);
      if (j+k == 0) {
        suit = this_suit;
      } else if (this_suit != suit) {
        return INVALID_GROUP_INDEX;
      }

      ranks[j+k] = deck_get_rank(cards[j+k]);
    }
  }
  return group_index_from_rank_array(m, group_size, ranks); 
}

group_index_t group_index_from_rank_array(size_t m, const uint8_t group_size[], const card_t ranks[]) {
  if (!group_index_valid(m, group_size)) {
    return false;
  }

  group_index_t index = 0, alpha = 1;
  rank_set_t used = EMPTY_RANK_SET;
  for(size_t i=0, j=0; i<m; j+=group_size[i++]) {
    rank_set_t this = EMPTY_RANK_SET;
    for(int k=0; k<group_size[i]; ++k) {
      if (!deck_valid_rank(ranks[j+k])) {
        return INVALID_GROUP_INDEX;
      }
      if (rank_set_is_set(this, ranks[j+k])) {
        return INVALID_GROUP_INDEX;
      }
      this = rank_set_set(this, ranks[j+k]);
    }
    
    group_index_t this_index = rank_set_index(this, used);
    if (this_index == INVALID_RANK_SET_INDEX) {
      return INVALID_GROUP_INDEX;
    }
    index += alpha*rank_set_index(this, used);
    alpha *= rank_set_index_size_from_count(group_size[i], j);
    used   = rank_set_union(this, used); 
  }

  return index;
}

bool group_unindex_to_card_array(size_t m, const uint8_t group_size[], group_index_t index, card_t suit, card_t cards[]) {
  if (!group_index_valid(m, group_size)) {
    return false;
  }
  if (!deck_valid_suit(suit)) {
    return false;
  }

  if (group_unindex_to_rank_array(m, group_size, index, cards)) {
    for(int i=0, j=0; i<m; j+=group_size[i++]) {
      for(int k=0; k<group_size[i]; ++k) {
        cards[j+k] = deck_make_card(suit, cards[j+k]);
      }
    }
    return true;
  }
  return false;
}

bool group_unindex_to_rank_array(size_t m, const uint8_t group_size[], group_index_t index, card_t ranks[]) {
  if (!group_index_valid(m, group_size)) {
    return false;
  }
  if (index >= group_index_size(m, group_size)) {
    return false;
  }

  rank_set_t used = EMPTY_RANK_SET;
  for(int i=0, j=0; i<m; j+=group_size[i++]) {
    rank_set_index_t this_size  = rank_set_index_size_from_count(group_size[i], j);
    if (this_size == INVALID_RANK_SET_INDEX) {
      return false;
    }
    
    rank_set_index_t this_index = index % this_size; index /= this_size;
    
    rank_set_t this_set = rank_set_unindex(group_size[i], this_index, used);
    if (this_set == INVALID_RANK_SET) {
      return false;
    }

    rank_set_to_rank_array(this_set, ranks+j);

    used = rank_set_union(used, this_set);
  }

  return true;
}

