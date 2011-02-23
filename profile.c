#include "cards.h"

int sort_n(const void* a,const void *b) 
{
    return *(int *)a - *(int *)b;
}

void hand_profile(Hand *hand)
{
    int i, j = 0;
    int buckets[13];
    char buf[2];
    make_rankings_histogram(hand, buckets);
    qsort(buckets, 13, sizeof(int), sort_n);
    for(i = 0; i < 13; i++) {
	if (buckets[i] > 0) {	    
	    hand->profile[j++] = buckets[i] + 48;
	}
    }
    hand->profile[j] = '\0';
}
