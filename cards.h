#include <stdio.h>
#include <stdlib.h>

#define RANK_N 13;

typedef struct {
    char *rank;
    char *suit;
} Card;

typedef struct {
    Card **cards;
    int len;
    char profile[6];
} Hand;

typedef struct {
    int mult;
    int kickers[4];
} Mult;

typedef struct {
    char ranking[20];
    int(*ranking_function)(Hand *);
    int(*chooser_function)(Hand *, Hand *);
} ranking_datum;

typedef int (*ranking_function)(Hand *);
typedef int (*chooser_function)(Hand *, Hand *);

Hand *sample_hand();
Hand *empty_hand();
void free_hand(Hand *);
void copy_cards(Hand *, Card **);
int straight_flush_chooser(Hand *, Hand *);
int fours_chooser(Hand *, Hand *);
int full_house_chooser(Hand *, Hand *);
int flush_chooser(Hand *, Hand *);
int straight_chooser(Hand *, Hand *);
int trips_chooser(Hand *, Hand *);
int two_pair_chooser(Hand *, Hand *);
int pair_chooser(Hand *, Hand *);
int high_card_chooser(Hand *, Hand *);
int *rank_hand(Hand *);
Card *high_card(Hand *hand);
char *hand_ranking_description(Hand *hand);
Hand *create_batch_hand(char *info);
Card *create_card(char *rank, char *suit);
Hand *create_hand();
void free_card(Card *cp);
void free_hand(Hand *hp);
void set_rank(Card *cp, char *rank);
void set_suit(Card *cp, char *suit);
Card *create_card_from_pretty(char *pretty);
void pretty_format_card(char *, Card *);
int card_compare_for_qsort(const void *, const void *);
int card_compare(Card *, Card *);
int is_5_high_straight(Hand *hand);
int card_lt(Card *cp1, Card *cp2);
int card_eq(Card *cp1, Card *cp2);
int card_gt(Card *cp1, Card *cp2);
int hand_has_nothing(Hand *hand);
int hand_has_pair(Hand *hand);
int hand_has_two_pair(Hand *hand);
int hand_has_three_of_a_kind(Hand *hand);
int hand_has_straight(Hand *hand);
int hand_has_flush(Hand *hand);
int hand_has_full_house(Hand *hand);
int hand_has_four_of_a_kind(Hand *hand);
int hand_has_straight_flush(Hand *hand);
int rank_difference(Card *cp1, Card *cp2);
int index_of_rank(char *rank);
int hand_n_of_a_kinds(Hand *hand, int n);
int rank_of_multiples(int[], int);
int highest_unmatched_card(Hand *, int[]);
int two_pair_hash(Hand *);
