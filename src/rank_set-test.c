#include "rank_set.h"
#include "test.h"

TEST(rank_set_size) {
  expect(rank_set_size(EMPTY_CARD_SET) == 0);
  expect(rank_set_from_rank(0) == 1);
  expect(rank_set_from_rank_array(3, (card_t[]){0,1,2}) == 3);
}

TEST(rank_set_valid) {
  for(rank_set_t i=0; i<(1u<<RANKS); ++i) {
    expect(rank_set_valid(i));
  }
}
