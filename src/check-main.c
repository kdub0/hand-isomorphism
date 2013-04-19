#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "hand_index.h"

static uint_fast32_t nth_bit(uint64_t used, uint8_t bit);

void test_full(hand_indexer_t * indexer) {
  uint_fast32_t total_cards = 0; for(uint_fast32_t i=0; i<indexer->rounds; ++i) {
    total_cards += indexer->cards_per_round[i];
  }
  uint_fast32_t permutations = 1; for(uint_fast32_t i=0; i<total_cards; ++i) {
    permutations *= CARDS-i;
  }
  
  hand_index_t size  = indexer->round_size[indexer->rounds-1];
  uint_fast32_t * seen = calloc(size, sizeof(uint_fast32_t)); assert(seen);
  
  uint8_t cards[7]; assert(total_cards <= 7);
  for(uint64_t i=0; i<permutations; ++i) {
    uint64_t p = i, used = 0;
    for(uint_fast32_t j=0; j<total_cards; ++j) {
      cards[j] = nth_bit(~used, p%(CARDS-j)); p /= CARDS-j;
      used    |= 1ull<<cards[j];
    } 

    hand_index_t index = hand_index_last(indexer, cards);
    assert(index < size);
    ++seen[index];
  }

  free(seen);

  for(uint64_t i=0; i<size; ++i) {
    hand_unindex(indexer, indexer->rounds-1, i, cards);
    assert(hand_index_last(indexer, cards) == i);
  }
}

void test_random(hand_indexer_t * indexer) {
  uint_fast32_t total_cards = 0; for(uint_fast32_t i=0; i<indexer->rounds; ++i) {
    total_cards += indexer->cards_per_round[i];
  }
  
  hand_index_t size  = indexer->round_size[indexer->rounds-1];
  
  uint8_t deck[CARDS], pi[SUITS];
  for(uint_fast32_t i=0; i<CARDS; ++i) {
    deck[i] = i;
  }
  for(uint_fast32_t i=0; i<SUITS; ++i) {
    pi[i] = i;
  }

  uint8_t cards[7]; assert(total_cards <= 7);
  for(uint64_t i=0; i<10000000; ++i) {
    for(uint_fast32_t i=0; i<total_cards; ++i) {
      uint_fast32_t j = rand()%(total_cards-i);
      uint8_t t = deck[i]; deck[i] = deck[i+j]; deck[i+j] = t;
    }
    for(uint_fast32_t i=0; i<SUITS; ++i) {
      uint_fast32_t j = rand()%(SUITS-i);
      uint8_t t = pi[i]; pi[i] = pi[i+j]; pi[i+j] = t;
    }
    memcpy(cards, deck, total_cards);
    for(uint_fast32_t i=0; i<total_cards; ++i) {
      cards[i] = deck_make_card(pi[deck_get_suit(cards[i])], deck_get_rank(cards[i]));
    }
    for(uint_fast32_t i=0, j=0; i<total_cards; ++j) {
      for(uint_fast32_t k=0; k<indexer->cards_per_round[j]; ++k, ++i) {
        uint_fast32_t ii = rand()%(indexer->cards_per_round[j]-k);
        uint8_t t = cards[i]; cards[i] = cards[i+ii]; cards[i+ii] = t;
      }
    }

    hand_index_t index  = hand_index_last(indexer, deck);
    hand_index_t index2 = hand_index_last(indexer, cards);
    assert(index < size);
    assert(index == index2);

    hand_unindex(indexer, indexer->rounds-1, index, cards);
    assert(hand_index_last(indexer, cards) == index);
  }
}

int main(int argc, char ** argv) {
  printf("testing hand-isomorphism...\n");  

  hand_indexer_t preflop_indexer;
  assert(hand_indexer_init(1, (uint8_t[]){2}, &preflop_indexer));

  hand_indexer_t flop_indexer;
  assert(hand_indexer_init(2, (uint8_t[]){2, 3}, &flop_indexer));

  hand_indexer_t turn_indexer;
  assert(hand_indexer_init(3, (uint8_t[]){2, 3, 1}, &turn_indexer));

  hand_indexer_t river_indexer;
  assert(hand_indexer_init(4, (uint8_t[]){2, 3, 1, 1}, &river_indexer));

  printf("sizes:" " %"PRIhand_index " %"PRIhand_index " %"PRIhand_index " %"PRIhand_index "\n",
      river_indexer.round_size[0], river_indexer.round_size[1],
      river_indexer.round_size[2], river_indexer.round_size[3]);

  printf("configurations:" " %"PRIuFAST32 " %"PRIuFAST32 " %"PRIuFAST32 " %"PRIuFAST32 "\n",
      river_indexer.configurations[0], river_indexer.configurations[1],
      river_indexer.configurations[2], river_indexer.configurations[3]);

  printf("permutations:" " %"PRIuFAST32 " %"PRIuFAST32 " %"PRIuFAST32 " %"PRIuFAST32 "\n",
      river_indexer.permutations[0], river_indexer.permutations[1],
      river_indexer.permutations[2], river_indexer.permutations[3]);

  assert(hand_indexer_size(&preflop_indexer, 0) == 169);
  assert(hand_indexer_size(&flop_indexer, 0) == 169);
  assert(hand_indexer_size(&turn_indexer, 0) == 169);
  assert(hand_indexer_size(&river_indexer, 0) == 169);
  assert(hand_indexer_size(&flop_indexer, 1) == 1286792);
  assert(hand_indexer_size(&turn_indexer, 1) == 1286792);
  assert(hand_indexer_size(&river_indexer, 1) == 1286792);
  assert(hand_indexer_size(&turn_indexer, 2) == 55190538);
  assert(hand_indexer_size(&river_indexer, 2) == 55190538);
  assert(hand_indexer_size(&river_indexer, 3) == 2428287420);

  uint8_t cards[7];
  printf("preflop table:\n");
  printf(" ");
  for(uint_fast32_t i=0; i<RANKS; ++i) {
    printf("  %c ", RANK_TO_CHAR[RANKS-1-i]);
  }
  printf("\n");
  for(uint_fast32_t i=0; i<RANKS; ++i) {
    printf("%c", RANK_TO_CHAR[RANKS-1-i]);
    for(uint_fast32_t j=0; j<RANKS; ++j) {
      cards[0] = deck_make_card(0, RANKS-1-j);
      cards[1] = deck_make_card(j<=i, RANKS-1-i);

      hand_index_t index = hand_index_last(&preflop_indexer, cards);
      printf(" %3" PRIhand_index, index);
    }
    printf("\n");
  }

  printf("full preflop...\n");
  test_full(&preflop_indexer);

  printf("full flop...\n");
  test_full(&flop_indexer);

  printf("random turn...\n");
  test_random(&turn_indexer);

  printf("random river...\n");
  test_random(&river_indexer);

  hand_indexer_free(&river_indexer);
  hand_indexer_free(&turn_indexer);
  hand_indexer_free(&flop_indexer);
  hand_indexer_free(&preflop_indexer);
}

static uint8_t nth_bit_[1<<16][16];
static void __attribute__((constructor)) nth_bit_ctor() {
  for(uint_fast32_t i=0; i<1<<16; ++i) {
    for(uint_fast32_t j=0, set=i; set; ++j, set&=set-1) {
      nth_bit_[i][j] = __builtin_ctz(set);
    }
  }
}

static uint_fast32_t nth_bit(uint64_t used, uint8_t bit) {
  for(uint_fast32_t i=0; i<4; ++i) {
    uint_fast32_t pop = __builtin_popcount(used&0xffff);
    if (pop > bit) {
      return 16*i + nth_bit_[used&0xffff][bit];
    } else {
      used >>= 16;
      bit   -= pop;
    }
  }
  return UINT_FAST32_MAX;
}
