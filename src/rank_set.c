#include <stdbool.h>
#include "rank_set.h"

/* FIXME: temporary */
static inline size_t nCr(size_t n, size_t r) {
  if (r <= n) { 
    if (n-r < r) {
      r = n-r;
    }
    size_t result = 1;
    for(int i=0; i<r; ++i) {
      result = result*(n-i)/(i+1);
    }
    return result;
  } else {
    return 0;
  }
}

size_t rank_set_size(rank_set_t set) {
  return __builtin_popcount(set);  
}

rank_set_t rank_set_from_rank_array(size_t n, const card_t ranks[]) {
  rank_set_t set = EMPTY_RANK_SET;
  for(size_t i=0; i<n; ++i) {
    if (!deck_valid_rank(ranks[i])) {
      return INVALID_RANK_SET;
    }
    if (rank_set_is_set(set, ranks[i])) {
      return INVALID_RANK_SET;
    }
    set = rank_set_add(set, ranks[i]);
  }
  return set;
}

rank_set_t rank_set_from_card_array(size_t n, const card_t cards[], card_t suit) {
  rank_set_t set = EMPTY_RANK_SET;
  for(size_t i=0; i<n; ++i) {
    if (!deck_valid_card(cards[i])) {
      return INVALID_RANK_SET;
    }
    if (deck_get_suit(cards[i]) == suit) {
      card_t rank = deck_get_rank(cards[i]);
      if (rank_set_is_set(set, rank)) {
        return INVALID_RANK_SET;
      }
      set = rank_set_add(set, rank);
    }
  }
  return set;
}

bool rank_set_to_rank_array(rank_set_t set, card_t ranks[]) {
  if (rank_set_valid(set)) {
    for(size_t i=0; !rank_set_empty(set); ++i) {
      ranks[i] = rank_set_next(&set);
    }
    return true;
  }
  return false;
}

bool rank_set_to_card_array(rank_set_t set, card_t cards[], card_t suit) {
  if (deck_valid_suit(suit) && rank_set_valid(set)) {
    for(size_t i=0; !rank_set_empty(set); ++i) {
      cards[i] = deck_make_card(suit, rank_set_next(&set));
    }
    return true;
  }
  return false;
}

bool rank_set_index_valid(size_t m, rank_set_index_t index, rank_set_t used) {
  return index != INVALID_RANK_SET_INDEX && index < rank_set_index_size(m, used);
}

rank_set_index_t rank_set_index_size(size_t m, rank_set_t used) {
  if (rank_set_valid(used)) {
    size_t used_size = rank_set_size(used);
    if (used_size+m <= RANKS) {
      return nCr(RANKS-used_size, m);
    }
  }
  return INVALID_RANK_SET_INDEX;
}

rank_set_index_t rank_set_index(rank_set_t set, rank_set_t used) {
  if (rank_set_valid(set) && rank_set_valid(used) && rank_set_intersect(set, used) == EMPTY_RANK_SET) {
    size_t m = rank_set_size(set);
    
    rank_set_index_t index = 0;
    for(size_t i=0; i<m; ++i) {
      card_t rank    = rank_set_next(&set);
      size_t smaller = rank_set_size(rank_set_intersect(rank_set_from_rank(rank)-1, used));
      if (rank-smaller >= i+1) {
        index       += nCr(rank-smaller, i+1); 
      }
    }

    return index;
  }
  return INVALID_RANK_SET_INDEX;
}

rank_set_t rank_set_unindex(size_t m, rank_set_index_t index, rank_set_t used) {
  if (rank_set_index_valid(m, index, used)) {
    /* FIXME: add the meat */
  }
  return INVALID_RANK_SET;
}


