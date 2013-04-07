#include "rank_set.h"
#include "test.h"

TEST(rank_set_size) {
  expect(rank_set_size(EMPTY_RANK_SET) == 0);
  expect(rank_set_from_rank(0) == 1);
  expect(rank_set_from_rank_array(3, (card_t[]){0,1,2}) == 3);
}

TEST(rank_set_valid) {
  for(rank_set_t i=0; i<(1u<<RANKS); ++i) {
    expect(rank_set_valid(i));
  }
}

TEST(rank_set_empty) {
  require(0 == EMPTY_RANK_SET);
  expect(rank_set_empty(EMPTY_RANK_SET));
  for(rank_set_t i=1; i<(1u<<RANKS); ++i) {
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
