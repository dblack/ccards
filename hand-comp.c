#include "cards.h"

/* The chooser methods. Each method implements logic for choosing between
   two hands with the same ranking. Pairs, trips, and fours are all
   handled by multiples_chooser. 
*/

int high_card_chooser(Hand *hand1, Hand *hand2)
{
    Card *high1 = high_card(hand1);
    Card *high2 = high_card(hand2);
    return index_of_rank(high1->rank) - index_of_rank(high2->rank);
}

static int high_kickers(int k1[], int k2[], int len)
{
    int i, j;
    for (i = 0; i < len; i++) {
	j = k1[i] - k2[i];
	if (j) {
	    return j;
	}
    }
    return 0;
}

static int multiples_chooser(Hand *hand1, Hand *hand2, int n)
{
    int buck1[13], buck2[13], result;
    Mult *mult1 = malloc(sizeof(Mult));
    Mult *mult2 = malloc(sizeof(Mult));
    n_of_a_kind_hash(hand1, mult1, 2);
    n_of_a_kind_hash(hand2, mult2, 2);

    result = ((mult1->mult - mult2->mult) ||
	      high_kickers(mult1->kickers, mult2->kickers, 3));

    free(mult1);
    free(mult2);

    return result;
}

int pair_chooser(Hand *hand1, Hand *hand2)
{
    multiples_chooser(hand1, hand2, 2);
}

int two_pair_chooser(Hand *hand1, Hand *hand2)
{
    return two_pair_hash(hand1) - two_pair_hash(hand2);
}

int trips_chooser(Hand *hand1, Hand *hand2)
{
    multiples_chooser(hand1, hand2, 3);
}

int straight_chooser(Hand *hand1, Hand *hand2)
{
    high_card_chooser(hand1, hand2);
}

int flush_chooser(Hand *hand1, Hand *hand2)
{
    high_card_chooser(hand1, hand2);
}

int full_house_chooser(Hand *hand1, Hand *hand2)
{
    int buck1[13], buck2[13];
    make_rankings_histogram(hand1, buck1);
    make_rankings_histogram(hand2, buck2);
    int pair1 = rank_of_multiples(buck1, 2);
    int trip1 = rank_of_multiples(buck1, 3);
    int pair2 = rank_of_multiples(buck2, 2);
    int trip2 = rank_of_multiples(buck2, 3);
    return (trip1 - trip2) || (pair1 - pair2);
}

int fours_chooser(Hand *hand1, Hand *hand2)
{
    return multiples_chooser(hand1, hand2, 4);
}

int straight_flush_chooser(Hand *hand1, Hand *hand2)
{
    return high_card_chooser(hand1, hand2);
}


/* Helper methods for the chooser logic */


/* Finds, e.g., the first occurrence of trips (n = 3) in the rank
   histogram of a hand.
*/

int rank_of_multiples(int rank_histogram[], int n)
{
    int i;
    for (i = 0; i < 13; i++) {
	if (rank_histogram[i] == n) {
	    return i;
	}
    }
    return -1;
}


/* Returns a three-digit hex number indicating the ranks of the high pair,
   the low pair, and the kicker. This can then be compared across hands to
   determine a winner between two two-pair hands.
*/

int two_pair_hash(Hand *hand)
{
    int i, hash, high = 0, low, kicker, buckets[13];
    char num[4];

    make_rankings_histogram(hand, buckets);

    for (i = 12; i >= 0; i--) {
	if (buckets[i] == 2) {
	    if (high) {
		low = i;
	    }
	    else {
		high = i;
	    }
	}
	if (buckets[i] == 1) {
	    kicker = i;
	}
    }

    sprintf(num, "%x%x%x", high, low, kicker);
    sscanf(num, "%x", &hash);
    return hash; 
}


