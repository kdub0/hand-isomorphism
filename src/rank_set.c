#include <stdbool.h>
#include "rank_set.h"

static uint16_t set_to_index[RANK_SETS], rank_set_size_[RANK_SETS], index_to_set_base[RANK_SETS];
static uint16_t * index_to_set[RANKS+1];
static uint16_t used_to_index_mask[RANK_SETS][14];
static uint64_t nCr52[53][64];

static void __attribute__((constructor)) init_rank_set_tables() {
  /* FIXME: move this else where */
  nCr52[0][0] = 1;
  for(size_t n=1; n<=52; ++n) {
    nCr52[n][0] = 1;
    nCr52[n][n] = 1;
    for(size_t k=1; k<n; ++k) {
      nCr52[n][k] = nCr52[n-1][k-1] + nCr52[n-1][k];
    }
  }
  
  index_to_set[0] = index_to_set_base;
  for(size_t i=1; i<=RANKS; ++i) {
    index_to_set[i] = index_to_set[i-1] + rank_set_index_size(i-1, 0);
  }

  for(rank_set_t set=0; set<RANK_SETS; ++set) {
    rank_set_size_[set] = __builtin_popcount(set);
  }

  for(rank_set_t set=0; set<RANK_SETS; ++set) {
    rank_set_index_t index = rank_set_index_nCr_empty(set);
    size_t m = rank_set_size(set);

    set_to_index[set]      = index;
    index_to_set[m][index] = set;
  }

  for(rank_set_t used=0; used<RANK_SETS; ++used) {
    size_t i=rank_set_size(used)-1; for(rank_set_t rem=used; !rank_set_empty(rem); --i) {
      card_t rank = rank_set_next(&rem);
      used_to_index_mask[used][i] = rank_set_from_rank(rank) - 1;
    }
  }
}

static inline uint64_t nCr(size_t n, size_t r) {
  if (r <= n) {
    if (n <= 52) {
      return nCr52[n][r];
    }

    if (n-r < r) {
      r = n-r;
    }

    uint64_t result = 1;
    for(int i=0; i<r; ++i) {
      result = result*(n-i)/(i+1);
    }

    return result;
  } else {
    return 0;
  }
}

size_t rank_set_size(rank_set_t set) {
  assert(rank_set_valid(set));
  return rank_set_size_[set];
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

rank_set_index_t rank_set_index_nCr_empty(rank_set_t set) {
  if (rank_set_valid(set)) {
    size_t m = rank_set_size(set);
    
    rank_set_index_t index = 0;
    for(size_t i=0; i<m; ++i) {
      card_t rank = rank_set_next(&set);
      if (rank >= i+1) {
        index += nCr(rank, i+1); 
      }
    }

    return index;
  }
  return INVALID_RANK_SET_INDEX;
}

rank_set_index_t rank_set_index_nCr(rank_set_t set, rank_set_t used) {
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

rank_set_t rank_set_unindex_nCr(size_t m, rank_set_index_t index, rank_set_t used) {
  if (rank_set_index_valid(m, index, used)) {
    rank_set_t set = EMPTY_RANK_SET;
    for(size_t i=0; i<m; ++i) {
      card_t r = 0, count = 0;
      for(; rank_set_is_set(used, r); ++r) {}
      for(; nCr(count+1, m-i) <= index; ++count) {
        for(++r; rank_set_is_set(used, r); ++r) {}
      }
      if (count >= m-i) {
        index -= nCr(count, m-i);
      }
      set = rank_set_set(set, r);
    }
    return set;
  }
  return INVALID_RANK_SET;
}

rank_set_index_t rank_set_index_empty(rank_set_t set) {
  if (rank_set_valid(set)) {
    return set_to_index[set];
  }
  return INVALID_RANK_SET_INDEX;
}

#define define_rank_set_shift_down(m) \
static rank_set_t rank_set_shift_down_##m(rank_set_t set, rank_set_t used) { \
  for(size_t i=0; i<m; ++i) { \
    rank_set_t mask = used_to_index_mask[used][i]; \
    rank_set_t low = set & mask, high = set & ~mask;\
    set = high>>1 | low;\
  } \
  return set; \
}

define_rank_set_shift_down(0);
define_rank_set_shift_down(1);
define_rank_set_shift_down(2);
define_rank_set_shift_down(3);
define_rank_set_shift_down(4);
define_rank_set_shift_down(5);
define_rank_set_shift_down(6);
define_rank_set_shift_down(7);
define_rank_set_shift_down(8);
define_rank_set_shift_down(9);
define_rank_set_shift_down(10);
define_rank_set_shift_down(11);
define_rank_set_shift_down(12);
define_rank_set_shift_down(13);

static rank_set_t (*rank_set_shift_down[14])(rank_set_t, rank_set_t) = {
  rank_set_shift_down_0,
  rank_set_shift_down_1,
  rank_set_shift_down_2,
  rank_set_shift_down_3,
  rank_set_shift_down_4,
  rank_set_shift_down_5,
  rank_set_shift_down_6,
  rank_set_shift_down_7,
  rank_set_shift_down_8,
  rank_set_shift_down_9,
  rank_set_shift_down_10,
  rank_set_shift_down_11,
  rank_set_shift_down_12,
  rank_set_shift_down_13,
};

#define define_rank_set_shift_up(n) \
static rank_set_t rank_set_shift_up_##n(rank_set_t set, rank_set_t used) { \
  for(ptrdiff_t i=n-1; i>=0; --i) { \
    rank_set_t mask = used_to_index_mask[used][i]; \
    rank_set_t low = set & mask, high = set & ~mask; \
    set = high<<1 | low; \
  } \
  return set; \
}

define_rank_set_shift_up(0);
define_rank_set_shift_up(1);
define_rank_set_shift_up(2);
define_rank_set_shift_up(3);
define_rank_set_shift_up(4);
define_rank_set_shift_up(5);
define_rank_set_shift_up(6);
define_rank_set_shift_up(7);
define_rank_set_shift_up(8);
define_rank_set_shift_up(9);
define_rank_set_shift_up(10);
define_rank_set_shift_up(11);
define_rank_set_shift_up(12);
define_rank_set_shift_up(13);

static rank_set_t (*rank_set_shift_up[14])(rank_set_t, rank_set_t) = {
  rank_set_shift_up_0,
  rank_set_shift_up_1,
  rank_set_shift_up_2,
  rank_set_shift_up_3,
  rank_set_shift_up_4,
  rank_set_shift_up_5,
  rank_set_shift_up_6,
  rank_set_shift_up_7,
  rank_set_shift_up_8,
  rank_set_shift_up_9,
  rank_set_shift_up_10,
  rank_set_shift_up_11,
  rank_set_shift_up_12,
  rank_set_shift_up_13,
};

rank_set_index_t rank_set_index(rank_set_t set, rank_set_t used) {
  if (rank_set_valid(set) && rank_set_valid(used) && rank_set_intersect(set, used) == EMPTY_RANK_SET) {
    return rank_set_index_empty(rank_set_shift_down[rank_set_size(used)](set, used));
  }
  return INVALID_RANK_SET_INDEX;
}

rank_set_t rank_set_unindex(size_t m, rank_set_index_t index, rank_set_t used) {
  if (rank_set_index_valid(m, index, used)) {
    return rank_set_shift_up[rank_set_size(used)](index_to_set[m][index], used);
  }
  return INVALID_RANK_SET;
}



