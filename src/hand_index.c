#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "hand_index.h"

#define MAX_GROUP_INDEX        0x100000 
#define MAX_CARDS_PER_ROUND    15
#define ROUND_SHIFT            4
#define ROUND_MASK             0xf

static uint8_t nth_unset[1<<RANKS][RANKS];
static bool equal[1<<(SUITS-1)][SUITS];
static uint_fast32_t nCr_ranks[RANKS+1][RANKS+1], rank_set_to_index[1<<RANKS], index_to_rank_set[RANKS+1][1<<RANKS], (*suit_permutations)[SUITS];
static hand_index_t nCr_groups[MAX_GROUP_INDEX][SUITS+1];
static void __attribute__((constructor)) hand_index_ctor() {
  for(uint_fast32_t i=0; i<1<<(SUITS-1); ++i) {
    for(uint_fast32_t j=1; j<SUITS; ++j) {
      equal[i][j] = i&1<<(j-1);
    }
  }

  for(uint_fast32_t i=0; i<1<<RANKS; ++i) {
    for(uint_fast32_t j=0, set=~i&(1<<RANKS)-1; j<RANKS; ++j, set&=set-1) {
      nth_unset[i][j] = set?__builtin_ctz(set):0xff;
    }
  }

  nCr_ranks[0][0]     = 1;
  for(uint_fast32_t i=1; i<RANKS+1; ++i) {
    nCr_ranks[i][0]   = nCr_ranks[i][i] = 1;
    for(uint_fast32_t j=1; j<i; ++j) {
      nCr_ranks[i][j] = nCr_ranks[i-1][j-1] + nCr_ranks[i-1][j];
    }
  }

  nCr_groups[0][0] = 1;
  for(uint_fast32_t i=1; i<MAX_GROUP_INDEX; ++i) {
    nCr_groups[i][0] = 1;
    if (i < SUITS+1) {
      nCr_groups[i][i] = 1;
    }
    for(uint_fast32_t j=1; j<(i<(SUITS+1)?i:(SUITS+1)); ++j) {
      nCr_groups[i][j] = nCr_groups[i-1][j-1] + nCr_groups[i-1][j];
    } 
  }

  for(uint_fast32_t i=0; i<1<<RANKS; ++i) {
    for(uint_fast32_t set=i, j=1; set; ++j, set&=set-1) {
      rank_set_to_index[i]  += nCr_ranks[__builtin_ctz(set)][j];
    }
    index_to_rank_set[__builtin_popcount(i)][rank_set_to_index[i]] = i;
  }

  uint_fast32_t num_permutations = 1;
  for(uint_fast32_t i=2; i<=SUITS; ++i) {
    num_permutations *= i;
  }

  suit_permutations = calloc(num_permutations, SUITS*sizeof(uint_fast32_t));
  
  for(uint_fast32_t i=0; i<num_permutations; ++i) {
    for(uint_fast32_t j=0, index=i, used=0; j<SUITS; ++j) {
      uint_fast32_t suit = index%(SUITS-j); index /= SUITS-j;
      uint_fast32_t shifted_suit = nth_unset[used][suit];
      suit_permutations[i][j] = shifted_suit;
      used                   |= 1<<shifted_suit;
    }
  }
} 

void enumerate_configurations_r(uint_fast32_t rounds, const uint8_t cards_per_round[], 
    uint_fast32_t round, uint_fast32_t remaining, 
    uint_fast32_t suit, uint_fast32_t equal, uint_fast32_t used[], uint_fast32_t configuration[],
    void (*observe)(uint_fast32_t, uint_fast32_t[], void*), void * data) {
  if (suit == SUITS) {
    observe(round, configuration, data);

    if (round+1 < rounds) {
      enumerate_configurations_r(rounds, cards_per_round, round+1, cards_per_round[round+1], 0, equal, used, configuration, observe, data);
    }
  } else {
    uint_fast32_t min = 0;
    if (suit == SUITS-1) {
      min = remaining;
    }
    
    uint_fast32_t max = RANKS-used[suit];
    if (remaining < max) {
      max = remaining;
    }
   
    uint_fast32_t previous = RANKS+1;
    bool was_equal = equal&1<<suit;
    if (was_equal) {
      previous = configuration[suit-1]>>ROUND_SHIFT*(rounds-round-1)&ROUND_MASK;
      if (previous < max) {
        max = previous;
      }
    }
    
    uint_fast32_t old_configuration = configuration[suit], old_used = used[suit];
    for(uint_fast32_t i=min; i<=max; ++i) {
      uint_fast32_t new_configuration = old_configuration | i<<ROUND_SHIFT*(rounds-round-1);
      uint_fast32_t new_equal = (equal&~(1<<suit))|(was_equal&(i==previous))<<suit;

      used[suit] = old_used+i;
      configuration[suit] = new_configuration;
      enumerate_configurations_r(rounds, cards_per_round, round, remaining-i, suit+1, new_equal, used, configuration, observe, data);
      configuration[suit] = old_configuration;
      used[suit] = old_used;
    }
  }
}

void enumerate_configurations(uint_fast32_t rounds, const uint8_t cards_per_round[],
    void (*observe)(uint_fast32_t, uint_fast32_t[], void*), void * data) {
  uint_fast32_t used[SUITS] = {0}, configuration[SUITS] = {0};
  enumerate_configurations_r(rounds, cards_per_round, 0, cards_per_round[0], 0, (1<<SUITS) - 2, used, configuration, observe, data);
}

void count_configurations(uint_fast32_t round, uint_fast32_t configuration[], void * data) {
  uint_fast32_t * counts = data; ++counts[round];
}

void tabulate_configurations(uint_fast32_t round, uint_fast32_t configuration[], void * data) {
  hand_indexer_t * indexer = data;

  uint_fast32_t id = indexer->configurations[round]++;
  for(; id>0; --id) {
    for(uint_fast32_t i=0; i<SUITS; ++i) {
      if (configuration[i] < indexer->configuration[round][id-1][i]) {
        break;
      } else if (configuration[i] > indexer->configuration[round][id-1][i]) {
        goto out;
      }
    }
    for(uint_fast32_t i=0; i<SUITS; ++i) {
      indexer->configuration[round][id][i]              = indexer->configuration[round][id-1][i];
      indexer->configuration_to_suit_size[round][id][i] = indexer->configuration_to_suit_size[round][id-1][i];
    }
    indexer->configuration_to_offset[round][id] = indexer->configuration_to_offset[round][id-1];
    indexer->configuration_to_equal[round][id]  = indexer->configuration_to_equal[round][id-1];
  }
out:;

  indexer->configuration_to_offset[round][id] = 1; 
  for(uint_fast32_t i=0; i<SUITS; ++i) {
    indexer->configuration[round][id][i] = configuration[i];
  }

  uint_fast32_t equal = 0;
  for(uint_fast32_t i=0; i<SUITS;) {
    hand_index_t size = 1;
    for(uint_fast32_t j=0, remaining=RANKS; j<=round; ++j) {
      uint_fast32_t ranks = configuration[i]>>ROUND_SHIFT*(indexer->rounds-j-1)&ROUND_MASK;
      size *= nCr_ranks[remaining][ranks];
      remaining -= ranks;
    }
    assert(size+SUITS-1 < MAX_GROUP_INDEX);
    
    uint_fast32_t j=i+1; for(; j<SUITS && configuration[j] == configuration[i]; ++j) {} 
    for(uint_fast32_t k=i; k<j; ++k) {
      indexer->configuration_to_suit_size[round][id][k] = size;
    }

    indexer->configuration_to_offset[round][id] *= nCr_groups[size+j-i-1][j-i];
    
    for(uint_fast32_t k=i+1; k<j; ++k) {
      equal |= 1<<k;
    }

    i = j;
  }
  
  indexer->configuration_to_equal[round][id] = equal>>1;
}

void enumerate_permutations_r(uint_fast32_t rounds, const uint8_t cards_per_round[], 
    uint_fast32_t round, uint_fast32_t remaining, 
    uint_fast32_t suit, uint_fast32_t used[], uint_fast32_t count[],
    void (*observe)(uint_fast32_t, uint_fast32_t[], void*), void * data) {
  if (suit == SUITS) {
    observe(round, count, data);

    if (round+1 < rounds) {
      enumerate_permutations_r(rounds, cards_per_round, round+1, cards_per_round[round+1], 0, used, count, observe, data);
    }
  } else {
    uint_fast32_t min = 0;
    if (suit == SUITS-1) {
      min = remaining;
    }
    uint_fast32_t max = RANKS-used[suit];
    if (remaining < max) {
      max = remaining;
    }
    
    uint_fast32_t old_count = count[suit], old_used = used[suit];
    for(uint_fast32_t i=min; i<=max; ++i) {
      uint_fast32_t new_count = old_count | i<<ROUND_SHIFT*(rounds-round-1);

      used[suit] = old_used+i;
      count[suit] = new_count;
      enumerate_permutations_r(rounds, cards_per_round, round, remaining-i, suit+1, used, count, observe, data);
      count[suit] = old_count;
      used[suit] = old_used;
    }
  }
}

void enumerate_permutations(uint_fast32_t rounds, const uint8_t cards_per_round[],
    void (*observe)(uint_fast32_t, uint_fast32_t[], void*), void * data) {
  uint_fast32_t used[SUITS] = {0}, count[SUITS] = {0};
  enumerate_permutations_r(rounds, cards_per_round, 0, cards_per_round[0], 0, used, count, observe, data);
}

void count_permutations(uint_fast32_t round, uint_fast32_t count[], void * data) {
  hand_indexer_t * indexer = data;

  uint_fast32_t idx = 0, mult = 1;
  for(uint_fast32_t i=0; i<=round; ++i) {
    for(uint_fast32_t j=0, remaining=indexer->cards_per_round[i]; j<SUITS-1; ++j) {
      uint_fast32_t size = count[j]>>(indexer->rounds-i-1)*ROUND_SHIFT&ROUND_MASK;
      idx  += mult*size;
      mult *= remaining+1;
      remaining -= size;
    }
  }
  
  if (indexer->permutations[round] < idx+1) {
    indexer->permutations[round] = idx+1;
  }
}

void tabulate_permutations(uint_fast32_t round, uint_fast32_t count[], void * data) {
  hand_indexer_t * indexer = data;

  uint_fast32_t idx = 0, mult = 1;
  for(uint_fast32_t i=0; i<=round; ++i) {
    for(uint_fast32_t j=0, remaining=indexer->cards_per_round[i]; j<SUITS-1; ++j) {
      uint_fast32_t size = count[j]>>(indexer->rounds-i-1)*ROUND_SHIFT&ROUND_MASK;
      idx       += mult*size;
      mult      *= remaining+1;
      remaining -= size;
    }
  }
  
  uint_fast32_t pi[SUITS];
  for(uint_fast32_t i=0; i<SUITS; ++i) {
    pi[i] = i;
  }

  for(uint_fast32_t i=1; i<SUITS; ++i) {
    uint_fast32_t j=i, pi_i = pi[i]; for(; j>0; --j) {
      if (count[pi_i] > count[pi[j-1]]) {
        pi[j] = pi[j-1];
      } else {
        break;
      }
    }
    pi[j] = pi_i;
  }

  uint_fast32_t pi_idx = 0, pi_mult = 1, pi_used = 0;
  for(uint_fast32_t i=0; i<SUITS; ++i) {
    uint_fast32_t this_bit = 1<<pi[i];
    uint_fast32_t smaller  = __builtin_popcount((this_bit-1)&pi_used);
    pi_idx  += (pi[i]-smaller)*pi_mult;
    pi_mult *= SUITS-i;
    pi_used |= this_bit;
  }

  indexer->permutation_to_pi[round][idx] = pi_idx;

  uint_fast32_t low = 0, high = indexer->configurations[round];
  while(low < high) {
    uint_fast32_t mid = (low+high)/2;

    int_fast32_t compare = 0;
    for(uint_fast32_t i=0; i<SUITS; ++i) {
      uint_fast32_t this  = count[pi[i]];
      uint_fast32_t other = indexer->configuration[round][mid][i];
      if (other > this) {
        compare = -1; break;
      } else if (other < this) {
        compare = 1; break;
      }
    }
 
    if (compare == -1) {
      high = mid;
    } else if (compare == 0) {
      low = high = mid;
    } else {
      low = mid+1;
    }
  }

  indexer->permutation_to_configuration[round][idx] = low;
}

bool hand_indexer_init(uint_fast32_t rounds, const uint8_t cards_per_round[], hand_indexer_t * indexer) {
  if (rounds == 0) {
    return false;
  }
  if (rounds > MAX_ROUNDS) {
    return false;
  }
  for(uint_fast32_t i=0, count=0; i<rounds; ++i) {
    count += cards_per_round[i];
    if (count > CARDS) {
      return false;
    }
  }

  memset(indexer, 0, sizeof(hand_indexer_t));

  indexer->rounds = rounds;
  memcpy(indexer->cards_per_round, cards_per_round, rounds); 
  for(uint_fast32_t i=0, j=0; i<rounds; ++i) {
    indexer->round_start[i] = j; j += cards_per_round[i];
  }

  memset(indexer->configurations, 0, sizeof(indexer->configurations));
  enumerate_configurations(rounds, cards_per_round, count_configurations, indexer->configurations);

  for(uint_fast32_t i=0; i<rounds; ++i) {
    indexer->configuration_to_equal[i]     = calloc(indexer->configurations[i], sizeof(uint_fast32_t));
    indexer->configuration_to_offset[i]    = calloc(indexer->configurations[i], sizeof(hand_index_t));
    indexer->configuration[i]              = calloc(indexer->configurations[i], SUITS*sizeof(uint_fast32_t));
    indexer->configuration_to_suit_size[i] = calloc(indexer->configurations[i], SUITS*sizeof(uint_fast32_t));
    if (!indexer->configuration_to_equal[i] ||
        !indexer->configuration_to_offset[i] ||
        !indexer->configuration[i] ||
        !indexer->configuration_to_suit_size[i]) {
      hand_indexer_free(indexer);
      return false; 
    }
  }

  memset(indexer->configurations, 0, sizeof(indexer->configurations));
  enumerate_configurations(rounds, cards_per_round, tabulate_configurations, indexer);
  
  for(uint_fast32_t i=0; i<rounds; ++i) {
    hand_index_t accum = 0; for(uint_fast32_t j=0; j<indexer->configurations[i]; ++j) {
      hand_index_t next = accum + indexer->configuration_to_offset[i][j];
      indexer->configuration_to_offset[i][j] = accum;
      accum = next;
    }
    indexer->round_size[i] = accum;
  }

  memset(indexer->permutations, 0, sizeof(indexer->permutations));
  enumerate_permutations(rounds, cards_per_round, count_permutations, indexer);
  
  for(uint_fast32_t i=0; i<rounds; ++i) {
    indexer->permutation_to_configuration[i] = calloc(indexer->permutations[i], sizeof(uint_fast32_t));
    indexer->permutation_to_pi[i] = calloc(indexer->permutations[i], sizeof(uint_fast32_t));
    if (!indexer->permutation_to_configuration ||
        !indexer->permutation_to_pi) {
      hand_indexer_free(indexer);
      return false; 
    }
  }

  enumerate_permutations(rounds, cards_per_round, tabulate_permutations, indexer);

  return true;
}

void hand_indexer_free(hand_indexer_t * indexer) {
  for(uint_fast32_t i=0; i<indexer->rounds; ++i) {
    free(indexer->permutation_to_configuration[i]);
    free(indexer->permutation_to_pi[i]);
    free(indexer->configuration_to_equal[i]);
    free(indexer->configuration_to_offset[i]);
    free(indexer->configuration[i]);
    free(indexer->configuration_to_suit_size[i]);
  }
}

hand_index_t hand_indexer_size(const hand_indexer_t * indexer, uint_fast32_t round) {
  assert(round < indexer->rounds);
  return indexer->round_size[round];
}

void hand_indexer_state_init(const hand_indexer_t * indexer, hand_indexer_state_t * state) {
  memset(state, 0, sizeof(hand_indexer_state_t));
 
  state->permutation_multiplier = 1;
  for(uint_fast32_t i=0; i<SUITS; ++i) {
    state->suit_multiplier[i] = 1;
  }
}

hand_index_t hand_index_all(const hand_indexer_t * indexer, const uint8_t cards[], hand_index_t indices[]) {
  if (indexer->rounds) {
    hand_indexer_state_t state; hand_indexer_state_init(indexer, &state);

    for(uint_fast32_t i=0, j=0; i<indexer->rounds; j+=indexer->cards_per_round[i++]) {
      indices[i] = hand_index_next_round(indexer, cards+j, &state);
    }

    return indices[indexer->rounds-1];
  }

  return 0;
}

hand_index_t hand_index_last(const hand_indexer_t * indexer, const uint8_t cards[]) {
  hand_index_t indices[MAX_ROUNDS];
  return hand_index_all(indexer, cards, indices);
}

hand_index_t hand_index_next_round(const hand_indexer_t * indexer, const uint8_t cards[], hand_indexer_state_t * state) {
  uint_fast32_t round = state->round++;
  assert(round < indexer->rounds);

  uint_fast32_t ranks[SUITS] = {0}, shifted_ranks[SUITS] = {0};
  for(uint_fast32_t i=0; i<indexer->cards_per_round[round]; ++i) {
    assert(cards[i] < CARDS);                 /* valid card */

    uint_fast32_t rank         = deck_get_rank(cards[i]), suit = deck_get_suit(cards[i]), rank_bit = 1<<rank;
    assert(!(ranks[suit]&rank_bit));
    ranks[suit]               |= rank_bit;
    shifted_ranks[suit]       |= rank_bit>>__builtin_popcount((rank_bit-1)&state->used_ranks[suit]);
  }

  for(uint_fast32_t i=0; i<SUITS; ++i) {
    assert(!(state->used_ranks[i]&ranks[i])); /* no duplicate cards */

    uint_fast32_t used_size    = __builtin_popcount(state->used_ranks[i]), this_size = __builtin_popcount(ranks[i]);
    state->suit_index[i]      += state->suit_multiplier[i]*rank_set_to_index[shifted_ranks[i]];
    state->suit_multiplier[i] *= nCr_ranks[RANKS-used_size][this_size];
    state->used_ranks[i]      |= ranks[i];
  }

  for(uint_fast32_t i=0, remaining=indexer->cards_per_round[round]; i<SUITS-1; ++i) {
    uint_fast32_t this_size          = __builtin_popcount(ranks[i]);
    state->permutation_index        += state->permutation_multiplier*this_size;
    state->permutation_multiplier   *= remaining+1;
    remaining                       -= this_size;
  }

  uint_fast32_t configuration = indexer->permutation_to_configuration[round][state->permutation_index];
  uint_fast32_t pi_index      = indexer->permutation_to_pi[round][state->permutation_index];
  uint_fast32_t equal_index   = indexer->configuration_to_equal[round][configuration];
  hand_index_t offset         = indexer->configuration_to_offset[round][configuration];
  const uint_fast32_t * pi    = suit_permutations[pi_index];

  hand_index_t suit_index[SUITS], suit_multiplier[SUITS];
  for(uint_fast32_t i=0; i<SUITS; ++i) {
    suit_index[i]      = state->suit_index[pi[i]]; 
    suit_multiplier[i] = state->suit_multiplier[pi[i]];
  }
  
  /* sort using an optimal sorting network */
#define swap(u, v) \
  do {\
    if (suit_index[u] > suit_index[v]) {\
      suit_index[u] ^= suit_index[v];\
      suit_index[v] ^= suit_index[u];\
      suit_index[u] ^= suit_index[v];\
    }\
  } while(0)

  hand_index_t index = offset, multiplier = 1;
  for(uint_fast32_t i=0; i<SUITS;) {
    hand_index_t part, size;

    if (i+1 < SUITS && equal[equal_index][i+1]) {
      if (i+2 < SUITS && equal[equal_index][i+2]) {
        if (i+3 < SUITS && equal[equal_index][i+3]) {
          /* four equal suits */
          swap(i, i+1); swap(i+2, i+3); swap(i, i+2); swap(i+1, i+3); swap(i+1, i+2);
          part = suit_index[i] + nCr_groups[suit_index[i+1]+1][2] + nCr_groups[suit_index[i+2]+2][3] + nCr_groups[suit_index[i+3]+3][4];
          size = nCr_groups[suit_multiplier[i]+3][4];
          i += 4;
        } else {
          /* three equal suits */
          swap(i, i+1); swap(i, i+2); swap(i+1, i+2);
          part = suit_index[i] + nCr_groups[suit_index[i+1]+1][2] + nCr_groups[suit_index[i+2]+2][3];
          size = nCr_groups[suit_multiplier[i]+2][3];
          i += 3;
        }
      } else {
        /* two equal suits*/
        swap(i, i+1);
        part = suit_index[i] + nCr_groups[suit_index[i+1]+1][2];
        size = nCr_groups[suit_multiplier[i]+1][2];
        i += 2;
      }
    } else {
      /* no equal suits */
      part = suit_index[i];
      size = suit_multiplier[i];
      i += 1;
    }

    index      += multiplier*part;
    multiplier *= size;
  }

#undef swap

  return index;
}

bool hand_unindex(const hand_indexer_t * indexer, uint_fast32_t round, hand_index_t index, uint8_t cards[]) {
  if (round >= indexer->rounds || index >= indexer->round_size[round]) {
    return false;
  }

  uint_fast32_t low = 0, high = indexer->configurations[round], configuration_idx = 0;
  while(low < high) {
    uint_fast32_t mid = (low+high)/2;
    if (indexer->configuration_to_offset[round][mid] <= index) {
      configuration_idx = mid;
      low = mid+1;
    } else {
      high = mid;
    }
  }
  index -= indexer->configuration_to_offset[round][configuration_idx];

  hand_index_t suit_index[SUITS];
  for(uint_fast32_t i=0; i<SUITS;) {
    uint_fast32_t j=i+1; for(; j<SUITS && indexer->configuration[round][configuration_idx][j] == indexer->configuration[round][configuration_idx][i]; ++j) {}
    
    uint_fast32_t suit_size  = indexer->configuration_to_suit_size[round][configuration_idx][i];
    hand_index_t group_size  = nCr_groups[suit_size+j-i-1][j-i];
    hand_index_t group_index = index%group_size; index /= group_size;

    for(; i<j-1; ++i) {
      suit_index[i] = low = floor(exp(log(group_index)/(j-i) - 1 + log(j-i))-j-i); high = ceil(exp(log(group_index)/(j-i) + log(j-i))-j+i+1);
      if (high > suit_size) {
        high = suit_size;
      }
      if (high <= low) {
        low = 0;
      }
      while(low < high) {
        uint_fast32_t mid = (low+high)/2;
        if (nCr_groups[mid+j-i-1][j-i] <= group_index) {
          suit_index[i] = mid;
          low = mid+1;
        } else {
          high = mid;
        }
      }

      //for(suit_index[i]=0; nCr_groups[suit_index[i]+1+j-i-1][j-i] <= group_index; ++suit_index[i]) {}
      group_index -= nCr_groups[suit_index[i]+j-i-1][j-i]; 
    }

    suit_index[i] = group_index; ++i;
  }
  
  uint8_t location[MAX_ROUNDS]; memcpy(location, indexer->round_start, MAX_ROUNDS);
  for(uint_fast32_t i=0; i<SUITS; ++i) {
    uint_fast32_t used = 0, m = 0;
    for(uint_fast32_t j=0; j<indexer->rounds; ++j) {
      uint_fast32_t n              = indexer->configuration[round][configuration_idx][i]>>ROUND_SHIFT*(indexer->rounds-j-1)&ROUND_MASK;
      uint_fast32_t round_size     = nCr_ranks[RANKS-m][n]; m += n;
      uint_fast32_t round_idx      = suit_index[i]%round_size; suit_index[i] /= round_size;
      uint_fast32_t shifted_cards  = index_to_rank_set[n][round_idx], rank_set = 0;
      for(uint_fast32_t k=0; k<n; ++k) {
        uint_fast32_t shifted_card = shifted_cards&-shifted_cards; shifted_cards ^= shifted_card;
        uint_fast32_t card         = nth_unset[used][__builtin_ctz(shifted_card)]; rank_set |= 1<<card;
        cards[location[j]++]       = deck_make_card(i, card);
      }
      used |= rank_set;
    }
  }

  return true;
}


