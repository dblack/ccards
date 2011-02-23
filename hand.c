/* hand.c -- basic (poker) hand functionality

Example:

  Hand *hand1 = create_hand();
  add_card_to_hand(hand1, "3", "diamonds");

or:

  char *info = "A of spades, 2 of clubs, 3 of hearts, 3 of clubs, Q of hearts";
  Hand *hand1 = create_batch_hand(info);

Then:

  hand_rankinghand1);             // 6 (see list)
  hand_ranking_description(hand1)  // "two pair" (see list)
  
If another hand (hand2) has a flush, then:

  hand_tie(hand1, hand2);       // false (0)
  hand_beats_hand(hand1, hand2) // true (>0)  
  hand_beats_hand(hand2, hand1) // true (<0)  

The rank and beating logic is pegged to the array of function pointers
rank_functions, and the parallel array of verbal hand descriptions
rank_descriptions. 

*/

#include "cards.h"
#include <string.h>
extern char *ranks[];
extern char *suits[];
#define suit(n) hand->cards[n]->suit

ranking_datum ranking_data[] = {
    { "straight flush",
      hand_has_straight_flush,
      straight_flush_chooser },
    { "four of a kind",
      hand_has_four_of_a_kind,
      fours_chooser },
    { "full house",
      hand_has_full_house,
      full_house_chooser },
    { "flush",
      hand_has_flush,
      flush_chooser },
    { "straight",
      hand_has_straight,
      straight_chooser },
    { "three of a kind",
      hand_has_three_of_a_kind,
      trips_chooser },
    { "two pair",
      hand_has_two_pair,
      two_pair_chooser },
    { "pair",
      hand_has_pair,
      pair_chooser },
    { "nothing",
      hand_has_nothing,
      high_card_chooser }
};

static void init_hand(Hand **hpp)
{
    *hpp = malloc(sizeof(Hand));
    (*hpp)->cards = NULL;
    (*hpp)->len = 0;
}

Hand *create_hand()
{
    Hand *hp;
    init_hand(&hp);
}

void free_hand(Hand *hp)
{
    int i;
    for (i = 0; i < hp->len; i++) {
	free_card(hp->cards[i]);
    }
    free(hp->cards);
    free(hp);
}

Card *high_card(Hand *hand) {
    Card *copy[hand->len];
    copy_cards(hand, copy);
    int i;
    qsort(copy, 5, sizeof(Card *), card_compare_for_qsort);
    return copy[hand->len - 1];
}

int hand_ranking(Hand *hand)
{
    int i, r;
    for (i = 0; i < sizeof(ranking_data[i].ranking); i++) {
	r = (*ranking_data[i].ranking_function)(hand);
	if(r) {
	    return i;
	}
    }
}

char *hand_ranking_description(Hand *hand)
{
    return ranking_data[hand_ranking(hand)].ranking;
}

int ordinal_ranking(char *desc)
{
    int i;
    for (i = 0; i < 13; i++) {
	if (!strcmp(desc, ranking_data[i].ranking)) {
	    return i;
	}
    }
    return -1;
}

/* Subtract "backwards", because the order is tested highest to lowest */
int compare_hands(Hand *hand1, Hand *hand2) {
    int hand1_ranking = hand_ranking(hand1);
    int hand2_ranking = hand_ranking(hand2);
    int comp = hand2_ranking - hand1_ranking;
    return (comp == 0) ? (*ranking_data[hand1_ranking].chooser_function)(hand1, hand2) : comp;
}

int hand_beats_hand(Hand *hand1, Hand *hand2) {
    return compare_hands(hand1, hand2) > 0;
}

int hand_tie(Hand *hand1, Hand *hand2) {
    return compare_hands(hand1, hand2) == 0;
}

void add_card_to_hand(Hand *hand, char *rank, char *suit) {
    Card *card = create_card(rank, suit);
    hand->cards  = realloc(hand->cards, (hand->len + 1) * sizeof(Card *));
    hand->cards[hand->len] = card;
    hand->len++;
}

Hand *create_batch_hand(char *card_info)
{
    char rank[3], suit[10];
    Hand *hand = create_hand();
    char *cp = card_info;
    int i = 0;
    sscanf(cp, "%s of %[a-z]s\n", rank, suit);
    add_card_to_hand(hand, rank, suit);
    while(cp = strchr(cp+1, ',')) { 
 	sscanf(cp, ", %s of %[a-z]s\n", rank, suit);	 
 	add_card_to_hand(hand, rank, suit); 
    } 
    return hand;
}

void n_of_a_kind_hash(Hand *hand, Mult *mp, int n) 
{
    int i, k = 0;
    int buckets[13];
    make_rankings_histogram(hand, buckets);
    for (i = 0; i < 13; i++) {
	if (buckets[i] == n) {
	    mp->mult = i;
	}
	if (buckets[i] == 1) {
	    mp->kickers[k++] = i;
	}
    }
}

int make_rankings_histogram(Hand *hand, int buckets[]) {
    int i,j;
    Card **cp = hand->cards;
    for (i = 0; i < 13; i++) {
	buckets[i] = 0;
    }
    for (i = 0; i < 13; i++) {
	for (j = 0; j < hand->len; j++) {
	    if ((!strcmp(cp[j]->rank, ranks[i]))) {
		buckets[i]++;
	    }
	}
    }
}

int eval_profile(Hand *hand, char *profile) {
    hand_profile(hand);
    return (!strcmp(hand->profile, profile));
}
 
int hand_has_nothing(Hand *hand)
{
    eval_profile(hand, "11111");
}

int hand_has_pair(Hand *hand)
{
    eval_profile(hand, "1112");
}

int hand_has_two_pair(Hand *hand)
{
    eval_profile(hand, "122");
}

int hand_has_three_of_a_kind(Hand *hand)
{
    eval_profile(hand, "113");
}

void copy_cards(Hand *hand, Card **copy) {
    int i, n = hand->len;
    for (i = 0; i < n; i++) {
	copy[i] = hand->cards[i];
    }
}


int hand_has_straight(Hand *hand)
{
    hand_profile(hand);
    if (strcmp(hand->profile, "11111")) {
	return 0;
    }
    
    int i;
    Card *copy[hand->len];
    copy_cards(hand, copy);
    qsort(copy, hand->len, sizeof(Card *), card_compare_for_qsort);    

    return (all_cards_consecutive(copy));
}

int all_cards_consecutive(Card **cards) {
    return rank_difference(cards[4], cards[0]) == 4 || is_a_baby_straight(cards);;
}

int is_a_baby_straight(Card **cards) {
    return ((!strcmp(cards[0]->rank, "2")
	     &&(!strcmp(cards[4]->rank, "A"))
	     &&(rank_difference(cards[3], cards[0]) == 3))); 
}

int hand_has_flush(Hand *hand)
{
    int i;
    for (i = 1; i < hand->len; i++) 
	if (strcmp(suit(0), suit(i))) 
	    return 0;	
    return 1;
}

int hand_has_full_house(Hand *hand)
{
    eval_profile(hand, "23");
}

int hand_has_four_of_a_kind(Hand *hand)
{
    eval_profile(hand, "14");
}

int hand_has_straight_flush(Hand *hand)
{
    return hand_has_straight(hand) && hand_has_flush(hand);
}

