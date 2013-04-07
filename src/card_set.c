#include <stdbool.h>
#include "card_set.h"

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

size_t card_set_size(card_set_t set) {
  return __builtin_popcount(set);  
}

card_set_t card_set_from_rank_array(size_t n, const card_t ranks[]) {
  card_set_t set = EMPTY_CARD_SET;
  for(size_t i=0; i<n; ++i) {
    if (!deck_valid_rank(ranks[i])) {
      return INVALID_CARD_SET;
    }
    if (card_set_is_set(set, ranks[i])) {
      return INVALID_CARD_SET;
    }
    set = card_set_add(set, ranks[i]);
  }
  return set;
}

card_set_t card_set_from_card_array(size_t n, const card_t cards[], card_t suit) {
  card_set_t set = EMPTY_CARD_SET;
  for(size_t i=0; i<n; ++i) {
    if (!deck_valid_card(cards[i])) {
      return INVALID_CARD_SET;
    }
    if (deck_get_suit(cards[i]) == suit) {
      card_t rank = deck_get_rank(cards[i]);
      if (card_set_is_set(set, rank)) {
        return INVALID_CARD_SET;
      }
      set = card_set_add(set, rank);
    }
  }
  return set;
}

bool card_set_to_rank_array(card_set_t set, card_t ranks[]) {
  if (card_set_valid(set)) {
    for(size_t i=0; !card_set_empty(set); ++i) {
      ranks[i] = card_set_next(&set);
    }
    return true;
  }
  return INVALID_CARD_SET;
}

bool card_set_to_card_array(card_set_t set, card_t cards[], card_t suit) {
  if (deck_valid_suit(suit) && card_set_valid(set)) {
    for(size_t i=0; !card_set_empty(set); ++i) {
      cards[i] = deck_make_card(card_set_next(&set), suit);
    }
    return true;
  }
  return INVALID_CARD_SET;
}

bool card_set_index_valid(size_t m, card_set_index_t index, card_set_t used) {
  return index != INVALID_CARD_SET_INDEX && index < card_set_index_size(m, used);
}

card_set_index_t card_set_index_size(size_t m, card_set_t used) {
  if (card_set_valid(used)) {
    size_t used_size = card_set_size(used);
    if (used_size+m <= RANKS) {
      return nCr(RANKS-used_size, m);
    }
  }
  return INVALID_CARD_SET_INDEX;
}

card_set_index_t card_set_index(card_set_t set, card_set_t used) {
  if (card_set_valid(set) && card_set_valid(used) && card_set_intersect(set, used) == EMPTY_CARD_SET) {
    /* FIXME: add the meat */
  }
  return INVALID_CARD_SET_INDEX;
}

card_set_t card_set_unindex(size_t m, card_set_index_t index, card_set_t used) {
  if (card_set_index_valid(m, index, used)) {
    /* FIXME: add the meat */
  }
  return INVALID_CARD_SET;
}


