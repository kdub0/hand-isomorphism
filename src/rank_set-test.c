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
