#include "cards.h"
#include <string.h>


/* cards.c -- basic card implementation

Example:

  Card *card1 = create_card("3", "hearts");
  Card *card2 = create_card("4", "diamonds");
  Card *card3 = create_card("4", "spades");

  card_eq(card2, card3);   // true
  card_lt(card1, card2);   // true
  card_gt(card2, card1);   // true

There's a set_suit and set_rank API:

  set_rank(card1, "K");
  set_suit(card1, "clubs");

which is useful mainly if you create hands (see hand.c) and
want to manipulate the cards. (See test/cards.c for examples.)

To free a card:

  Card *card;
  // ...
  free_card(card);

*/

char *suits[] = { "clubs", "diamonds", "hearts", "spades" };
char *ranks[]  = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };

static void init_card(Card **cpp) {
    *cpp = malloc(sizeof(Card));
    Card *cp = *cpp;
    cp->rank = NULL;
    cp->suit = NULL;
}

Card *create_card(char *rank, char *suit)
{
    Card *cp;
    init_card(&cp);
    set_rank(cp, rank);
    set_suit(cp, suit);
    return cp;
}

void free_card(Card *cp)
{
    free(cp->rank);
    free(cp->suit);
    free(cp);
}

void set_rank(Card *cp, char *rank)
{
    cp->rank = realloc(cp->rank, strlen(rank) + 1);
    strcpy(cp->rank, rank);
}

void set_suit(Card *cp, char *suit)
{
    cp->suit = realloc(cp->suit, strlen(suit) + 1);
    strcpy(cp->suit, suit);
}

void pretty_format_card(char *buffer, Card *cp) {
    sprintf(buffer, "%s of %s", cp->rank, cp->suit);
}

int card_compare_for_qsort(const void *vp1, const void *vp2)
{
    Card *cp1 = *(Card **)vp1;
    Card *cp2 = *(Card **)vp2;
    return rank_difference(cp1, cp2);
}

int card_lt(Card *cp1, Card *cp2)
{
    return (rank_difference(cp1, cp2) < 0);
}

int card_eq(Card *cp1, Card *cp2)
{
    return (rank_difference(cp1, cp2) == 0);
}

int card_gt(Card *cp1, Card *cp2)
{
    return (rank_difference(cp1, cp2) > 0);
}

int rank_difference(Card *cp1, Card *cp2)
{
    return index_of_rank(cp1->rank) - index_of_rank(cp2->rank);
}

int index_of_rank(char *rank)
{
    int i;
    for (i = 0; i < 13; i++) {
	if (!strcmp(rank, ranks[i])) {
	    return i;
	}
    }
    return -1;
}

