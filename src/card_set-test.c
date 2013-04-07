#include "card_set.h"
#include "test.h"

TEST(card_set_size) {
  expect(card_set_size(EMPTY_CARD_SET) == 0);
  expect(card_set_from_card(0) == 1);
  expect(card_set_from_rank_array(3, (card_t[]){0,1,2}) == 3);
}
