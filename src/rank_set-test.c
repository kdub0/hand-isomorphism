#include <stdlib.h>
#include <string.h>
#include "rank_set.h"
#include "test.h"

TEST(rank_set_size) {
  expect(rank_set_size(EMPTY_RANK_SET) == 0);
  expect(rank_set_size(rank_set_from_rank(1)) == 1);
  expect(rank_set_size(rank_set_from_rank_array(3, (card_t[]){0,1,2})) == 3);
}

TEST(rank_set_from_rank_array) {
  expect(rank_set_from_rank_array(0, (card_t[]){}) == 0);
  expect(rank_set_from_rank_array(1, (card_t[]){0}) == 1);
  expect(rank_set_from_rank_array(2, (card_t[]){1,0}) == 3);
  expect(rank_set_from_rank_array(3, (card_t[]){2,1,0}) == 7);
  expect(rank_set_from_rank_array(1, (card_t[]){RANKS}) == INVALID_RANK_SET);
  expect(rank_set_from_rank_array(2, (card_t[]){1,1}) == INVALID_RANK_SET);
}

TEST(rank_set_from_card_array) {
  expect(rank_set_from_card_array(0, (card_t[]){}, 0) == 0);
  expect(rank_set_from_card_array(1, (card_t[]){0}, 0) == 1);
  expect(rank_set_from_card_array(2, (card_t[]){4,0}, 0) == 3);
  expect(rank_set_from_card_array(3, (card_t[]){8,4,0}, 0) == 7);
  expect(rank_set_from_card_array(1, (card_t[]){CARDS}, 0) == INVALID_RANK_SET);
  expect(rank_set_from_card_array(2, (card_t[]){4,4}, 0) == INVALID_RANK_SET);

  expect(rank_set_from_card_array(0, (card_t[]){}, 1) == 0);
  expect(rank_set_from_card_array(1, (card_t[]){0}, 1) == 0);
  expect(rank_set_from_card_array(2, (card_t[]){4,0}, 1) == 0);
  expect(rank_set_from_card_array(3, (card_t[]){8,4,0}, 1) == 0);
  expect(rank_set_from_card_array(1, (card_t[]){CARDS}, 1) == INVALID_RANK_SET);
  expect(rank_set_from_card_array(2, (card_t[]){4,4}, 1) == 0);

  expect(rank_set_from_card_array(3, (card_t[]){0,5,1}, 1) == 3);
  expect(rank_set_from_card_array(3, (card_t[]){5,5,0}, 0) == 1);
}

TEST(rank_set_to_rank_array) {
  card_t ranks[3];

  expect(!rank_set_to_rank_array(INVALID_RANK_SET, ranks));
  require(rank_set_to_rank_array(0, ranks));

  require(rank_set_to_rank_array(1, ranks));
  expect(ranks[0] == 0);
  
  require(rank_set_to_rank_array(2, ranks));
  expect(ranks[0] == 1);

  require(rank_set_to_rank_array(3, ranks));
  expect(ranks[0] == 0);
  expect(ranks[1] == 1);
}

TEST(rank_set_to_card_array) {
  card_t cards[3];

  expect(!rank_set_to_card_array(INVALID_RANK_SET, cards, 0));
  require(rank_set_to_card_array(0, cards, 0));

  require(rank_set_to_card_array(1, cards, 0));
  expect(cards[0] == 0);
  
  require(rank_set_to_card_array(2, cards, 0));
  expect(cards[0] == 4);

  require(rank_set_to_card_array(3, cards, 0));
  expect(cards[0] == 0);
  expect(cards[1] == 4);

  require(rank_set_to_card_array(1, cards, 1));
  expect(cards[0] == 1);
  
  require(rank_set_to_card_array(2, cards, 1));
  expect(cards[0] == 5);

  require(rank_set_to_card_array(3, cards, 1));
  expect(cards[0] == 1);
  expect(cards[1] == 5);
}

TEST(rank_set_index_valid) {
  rank_set_index_t size = rank_set_index_size_from_count(3, 1);
  for(rank_set_index_t i=0; i<size; ++i) {
    expect(rank_set_index_valid(3, i, 1));
  }
  expect(!rank_set_index_valid(3, size, 1));
  expect(!rank_set_index_valid(3, 5+size, 1));
}

TEST(rank_set_index_size_from_count) {
  expect(rank_set_index_size_from_count(2, 0) == 78);
  expect(rank_set_index_size_from_count(2, 1) == 66);
  expect(rank_set_index_size_from_count(2, 2) == 55);
  expect(rank_set_index_size_from_count(3, 0) == 286);
  expect(rank_set_index_size_from_count(3, 1) == 220);
  expect(rank_set_index_size_from_count(3, 2) == 165);
}

TEST(rank_set_index_size_from_used) {
  expect(rank_set_index_size_from_used(2, 0) == 78);
  expect(rank_set_index_size_from_used(2, 1) == 66);
  expect(rank_set_index_size_from_used(2, 2) == 66);
  expect(rank_set_index_size_from_used(2, 3) == 55);
  expect(rank_set_index_size_from_used(3, 0) == 286);
  expect(rank_set_index_size_from_used(3, 1) == 220);
  expect(rank_set_index_size_from_used(3, 2) == 220);
  expect(rank_set_index_size_from_used(3, 3) == 165);
}

TEST(rank_set_index_empty_match) {
  for(rank_set_t set=0; set<RANK_SETS; ++set) {
    require(rank_set_index_empty(set) == rank_set_index_nCr_empty(set));
  }
}

TEST(rank_set_index_match) {
  for(rank_set_t used=0; used<RANK_SETS; ++used) {
    for(rank_set_t set=0; set<RANK_SETS; ++set) {
      require(rank_set_index(set, used) == rank_set_index_nCr(set, used));
    }
  }
}

TEST(rank_set_unindex_match) {
  for(size_t used=0; used<RANK_SETS; ++used) {
    size_t used_size = rank_set_size(used);
    for(size_t m=0; m<=RANKS-used_size; ++m) {
      rank_set_index_t size = rank_set_index_size_from_used(m, used);
      for(rank_set_index_t idx=0; idx<size; ++idx) {
        require(rank_set_unindex(m, idx, used) == rank_set_unindex_nCr(m, idx, used));
      }
    }
  }
}

TEST(rank_set_index_all_available) {
  expect(rank_set_index(INVALID_RANK_SET, 0) == INVALID_RANK_SET_INDEX);
  expect(rank_set_index(0, INVALID_RANK_SET) == INVALID_RANK_SET_INDEX);
  
  rank_set_t * index_to_set = malloc(RANK_SETS*sizeof(rank_set_t)); require(index_to_set);

  /* loop over all sets sizes */
  for(size_t m=0; m<=RANKS; ++m) {
    memset(index_to_set, 0xff, RANK_SETS*sizeof(rank_set_t)); /* set to invalid */

    rank_set_index_t size = rank_set_index_size_from_count(m, 0), count = 0;
    require(size <= RANK_SETS);

    /* loop over all sets, make sure no collisions */
    for(rank_set_t set=0; set<RANK_SETS; ++set) {
      if (rank_set_size(set) == m) {
        rank_set_index_t index = rank_set_index(set, 0);
        require(index >= 0 && index < size);

        require(index_to_set[index] == INVALID_RANK_SET);
        index_to_set[index] = set;

        ++count;
      }
    }

    /* make sure counts line up */
    expect(count == size);
  }

  free(index_to_set);
}

TEST(rank_set_index_all) {
  expect(rank_set_index(INVALID_RANK_SET, 0) == INVALID_RANK_SET_INDEX);
  expect(rank_set_index(0, INVALID_RANK_SET) == INVALID_RANK_SET_INDEX);
  
  rank_set_t * index_to_set = malloc(RANK_SETS*sizeof(rank_set_t)); require(index_to_set);

  /* loop over all unavailable sets */
  for(rank_set_t used=0; used<RANK_SETS; ++used) {
    size_t used_size = rank_set_size(used);
    
    /* loop over all sets sizes */
    for(size_t m=0; m<=RANKS-used_size; ++m) {
      memset(index_to_set, 0xff, RANK_SETS*sizeof(rank_set_t)); /* set to invalid */

      rank_set_index_t size = rank_set_index_size_from_used(m, used), count = 0;
      require(size <= RANK_SETS);

      /* loop over all sets, make sure no collisions */
      for(rank_set_t set=0; set<RANK_SETS; ++set) {
        if (rank_set_size(set) == m) {
          rank_set_index_t index = rank_set_index(set, used);
          if (rank_set_intersect(set, used)) {
            expect(index == INVALID_RANK_SET_INDEX);
          } else {
            require(index >= 0 && index < size);

            require(index_to_set[index] == INVALID_RANK_SET);
            index_to_set[index] = set;

            ++count;
          }
        }
      }

      /* make sure counts line up */
      expect(count == size);
    }
  }

  free(index_to_set);
}

TEST(rank_set_unindex_all_available) {
  for(rank_set_t set=0; set<RANK_SETS; ++set) {
    size_t m = rank_set_size(set);
    require(set == rank_set_unindex(m, rank_set_index(set, 0), 0));
  }
}

TEST(rank_set_unindex_all) {
  for(rank_set_t used=0; used<RANK_SETS; ++used) {
    for(rank_set_t set=0; set<RANK_SETS; ++set) {
      if (!rank_set_intersect(set, used)) {
        size_t m = rank_set_size(set);
        require(set == rank_set_unindex(m, rank_set_index(set, used), used));
      }
    }
  }
}

TEST(rank_set_valid) {
  for(rank_set_t i=0; i<RANK_SETS; ++i) {
    expect(rank_set_valid(i));
  }
}

TEST(rank_set_empty) {
  require(0 == EMPTY_RANK_SET);
  expect(rank_set_empty(EMPTY_RANK_SET));
  for(rank_set_t i=1; i<RANK_SETS; ++i) {
    expect(!rank_set_empty(i));
  }
}

TEST(rank_set_is_singleton) {
  expect(!rank_set_is_singleton(0));
  expect(!rank_set_is_singleton(3));
  expect(rank_set_is_singleton(1));
  expect(rank_set_is_singleton(2));
}

TEST(rank_set_from_rank) {
  for(card_t i=0; i<RANKS; ++i) {
    expect(rank_set_from_rank(i) == 1u<<i);
  }
}

TEST(rank_from_rank_set) {
  for(card_t i=0; i<RANKS; ++i) {
    expect(rank_from_rank_set(rank_set_from_rank(i)) == i);
  }
}

TEST(rank_set_intersect) {
  expect(rank_set_intersect(1, 3) == 1);
  expect(rank_set_intersect(1, 2) == 0);
  expect(rank_set_intersect(7, 3) == 3);
}

TEST(rank_set_union) {
  expect(rank_set_union(1, 3) == 3);
  expect(rank_set_union(1, 2) == 3);
  expect(rank_set_union(7, 3) == 7);
}

TEST(rank_set_is_set) {
  expect(!rank_set_is_set(0, 0));
  expect(rank_set_is_set(1, 0));
  expect(!rank_set_is_set(2, 0));
  expect(!rank_set_is_set(2, 3));
  expect(rank_set_is_set(2, 1));
  expect(rank_set_is_set(3, 1));
}

TEST(rank_set_add) {
  expect(rank_set_add(0, 0) == 1);
  expect(rank_set_add(0, 1) == 2);
  expect(rank_set_add(2, 0) == 3);
  expect(rank_set_add(3, 2) == 7);
  expect(rank_set_add(1, 2) == 5);
}

TEST(rank_set_remove) {
  expect(rank_set_remove(1, 0) == 0);
  expect(rank_set_remove(3, 0) == 2);
  expect(rank_set_remove(7, 1) == 5);
  expect(rank_set_remove(7, 2) == 3);
  expect(rank_set_remove(7, 0) == 6);
}

TEST(rank_set_set) {
  expect(rank_set_set(0, 0) == 1);
  expect(rank_set_set(0, 1) == 2);
  expect(rank_set_set(2, 0) == 3);
  expect(rank_set_set(3, 2) == 7);
  expect(rank_set_set(1, 2) == 5);
  expect(rank_set_set(1, 0) == 1);
  expect(rank_set_set(3, 1) == 3);
}

TEST(rank_set_unset) {
  expect(rank_set_unset(1, 0) == 0);
  expect(rank_set_unset(3, 0) == 2);
  expect(rank_set_unset(7, 1) == 5);
  expect(rank_set_unset(7, 2) == 3);
  expect(rank_set_unset(7, 0) == 6);
  expect(rank_set_unset(0, 0) == 0);
  expect(rank_set_unset(2, 0) == 2);
  expect(rank_set_unset(5, 1) == 5);
}

TEST(rank_set_toggle) {
  expect(rank_set_toggle(1, 0) == 0);
  expect(rank_set_toggle(3, 0) == 2);
  expect(rank_set_toggle(7, 1) == 5);
  expect(rank_set_toggle(7, 2) == 3);
  expect(rank_set_toggle(7, 0) == 6);
  expect(rank_set_toggle(0, 0) == 1);
  expect(rank_set_toggle(2, 0) == 3);
  expect(rank_set_toggle(5, 1) == 7);
}

TEST(rank_set_first) {
  for(card_t i=0; i<RANKS-1; ++i) {
    expect(rank_set_first(rank_set_from_rank(i)) == i);
    expect(rank_set_first(rank_set_union(rank_set_from_rank(i+1), rank_set_from_rank(i))) == i);
  }
}

TEST(rank_set_next) {
  for(card_t i=0; i<RANKS-1; ++i) {
    rank_set_t set = rank_set_union(rank_set_from_rank(i+1), rank_set_from_rank(i));
    expect(rank_set_next(&set) == i); require(!rank_set_empty(set));
    expect(rank_set_next(&set) == i+1); expect(rank_set_empty(set));
  }
}
