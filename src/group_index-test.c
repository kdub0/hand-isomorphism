#include "group_index.h"
#include "test.h"

TEST(group_index_size) {
  expect(group_index_size(1, (uint8_t[]){1}) == RANKS);
  expect(group_index_size(1, (uint8_t[]){2}) == RANKS*(RANKS-1)/2);
  expect(group_index_size(2, (uint8_t[]){1,1}) == RANKS*(RANKS-1));
  expect(group_index_size(2, (uint8_t[]){2,1}) == RANKS*(RANKS-1)/2*(RANKS-2));
  expect(group_index_size(2, (uint8_t[]){2,2}) == RANKS*(RANKS-1)/2*(RANKS-2)*(RANKS-3)/2);
}

TEST(group_index) {
  expect(group_index_from_rank_array(1, (uint8_t[]){1}, (card_t[]){RANKS+1}) == INVALID_GROUP_INDEX);
  expect(group_index_from_card_array(1, (uint8_t[]){1}, (card_t[]){CARDS}) == INVALID_GROUP_INDEX);
  expect(group_index_from_rank_array(1, (uint8_t[]){2}, (card_t[]){0,0}) == INVALID_GROUP_INDEX);
  expect(group_index_from_rank_array(2, (uint8_t[]){1,1}, (card_t[]){0,0}) == INVALID_GROUP_INDEX);
}

TEST(group_unindex) {
  card_t ranks[RANKS];
  expect(!group_unindex_to_rank_array(1, (uint8_t[]){RANKS+1}, 0, ranks));
  expect(!group_unindex_to_rank_array(2, (uint8_t[]){2,RANKS-1}, 0, ranks));
  expect(!group_unindex_to_rank_array(1, (uint8_t[]){1}, RANKS, ranks));
  expect(!group_unindex_to_card_array(1, (uint8_t[]){1}, 0, 5, ranks));
}

TEST(group_index_enumerate) {
  uint8_t group_size[2] = {2,3};
  group_index_t size = group_index_size(2, group_size);

  card_t ranks[5], cards[5];
  for(group_index_t i=0; i<size; ++i) {
    require(group_unindex_to_rank_array(2, group_size, i, ranks));
    require(i == group_index_from_rank_array(2, group_size, ranks));

    require(group_unindex_to_card_array(2, group_size, i, i%SUITS, cards));
    require(i == group_index_from_card_array(2, group_size, cards));
  }
}
