#include "cards.h"
char *make_profile(Hand *, char *);
int main() {
    Hand *hand;
    hand = create_batch_hand("A of clubs, 3 of diamonds, 2 of hearts, 2 of spades, 2 of clubs");
    char pstr[6];
    char *profile = make_profile(hand, pstr); 
    printf("profile: %s\n", pstr);
}
