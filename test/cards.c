#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <CUnit/CUError.h>
#include "../cards.h"

static int my_suite_init(void) { return 0; }
static int my_suite_clean(void) { return 0; }

void report_ranks(Hand *hand) 
{
    int i;
    for (i = 0; i < 5; i++) printf("Rank %d: %s\n", i, hand->cards[i]->rank);
}

Hand *empty_hand() 
{
    return create_hand();
}

Hand *sample_hand()
{
    return create_batch_hand("3 of hearts, 4 of diamonds, 5 of spades, K of spades, 5 of clubs");
}

void test_pretty_formatting()
{
    char buffer[30];
    Card *cp = create_card("5", "hearts");
    pretty_format_card(buffer, cp);
    CU_ASSERT_STRING_EQUAL(buffer, "5 of hearts");
}

void test_card_gteqlt()
{
    Card *cp1 = create_card("3", "hearts");
    Card *cp2 = create_card("4", "diamonds");
    CU_ASSERT(card_lt(cp1, cp2));
    CU_ASSERT(card_eq(cp1, cp1));
    CU_ASSERT(card_gt(cp2, cp1));
    free_card(cp1);
    free_card(cp2);
}

void test_rank_index()
{
    CU_ASSERT_EQUAL(index_of_rank("5"), 3);
    CU_ASSERT_EQUAL(index_of_rank("Z"), -1);
}

void test_rank_difference()
{
    Card *cp1 = create_card("3", "hearts");
    Card *cp2 = create_card("6", "diamonds");
    CU_ASSERT_EQUAL(rank_difference(cp1, cp2), -3);
    CU_ASSERT_EQUAL(rank_difference(cp2, cp1), 3);
    free_card(cp1);
    free_card(cp2);
}

void test_reporting_rank_of_hand() 
{
    Hand *hp = sample_hand();
    char *result = hand_ranking_description(hp);
    CU_ASSERT(hand_has_pair(hp));
    CU_ASSERT(!strcmp(result, "pair"));
    free_hand(hp);
}

void test_create_hand_with_multiple_specs()
{
    char *specs = "A of diamonds, 3 of spades, 2 of hearts, 5 of clubs";
    Hand *hand = create_batch_hand(specs);
    CU_ASSERT_STRING_EQUAL(hand->cards[1]->suit, "spades");
    CU_ASSERT_STRING_EQUAL(hand->cards[0]->rank, "A");
    free_hand(hand);
}

void test_add_card_to_hand()
{
    Hand *hand = sample_hand();
    add_card_to_hand(hand, "3", "spades");
    Card *card = hand->cards[5];
    CU_ASSERT_STRING_EQUAL(card->suit, "spades");
    free_hand(hand);
}

void test_card_comparison()
{
    Hand *hand = create_batch_hand("4 of hearts, 3 of diamonds, 5 of spades, A of spades");

    Card **cards = hand->cards;
    CU_ASSERT(rank_difference(cards[0], cards[1]) > 0);  // 4 <=> 3
    CU_ASSERT(rank_difference(cards[1], cards[1]) == 0); // 3 <=> 3
    CU_ASSERT(rank_difference(cards[2], cards[1]) > 0);  // 5 <=> 3
    CU_ASSERT(rank_difference(cards[3], cards[2]) > 0);  // A <=> 5
    CU_ASSERT(rank_difference(cards[1], cards[3]) < 0);  // 3 <=> A

    free_hand(hand);
}

void test_n_of_a_kind()
{
    Hand *hp = sample_hand();
    Card **cpp = hp->cards;

    CU_ASSERT(hand_has_pair(hp));

    set_rank(cpp[0], "5");
    CU_ASSERT_FALSE(hand_has_pair(hp));

    set_rank(cpp[0], "7");
    set_rank(cpp[4], "J");
    CU_ASSERT_FALSE(hand_has_pair(hp));

    set_rank(cpp[0], "3");
    set_rank(cpp[1], "J");
    set_rank(cpp[2], "7");
    set_rank(cpp[3], "3");
    set_rank(cpp[4], "3");
    CU_ASSERT(hand_has_three_of_a_kind(hp));

    set_rank(cpp[2], "3");
    CU_ASSERT_FALSE(hand_has_three_of_a_kind(hp));
    CU_ASSERT(hand_has_four_of_a_kind(hp));
    CU_ASSERT_FALSE(hand_has_full_house(hp));

    set_rank(cpp[1], "7");
    set_rank(cpp[2], "7");
    CU_ASSERT(hand_has_full_house(hp));
    free_hand(hp);
}

void test_straight_flush()
{
    int i;
    char *ranks[] = {"3", "4", "5", "6", "7"};
    Hand *hp = sample_hand();
    for (i = 0; i < 5; i++) {
	set_suit(hp->cards[i], "hearts");
	set_rank(hp->cards[i], ranks[i]);
    }
    CU_ASSERT(hand_has_straight_flush(hp));
 
}

void test_flush()
{
    Hand *hp = sample_hand();
    int i;
    CU_ASSERT_FALSE(hand_has_flush(hp));
    for (i = 0; i < 5; i++) {
	set_suit(hp->cards[i], "hearts");
    }
    
    CU_ASSERT(hand_has_flush(hp));
}

void test_straights()
{  
    Hand *hp = create_batch_hand("3 of hearts, 4 of clubs, 5 of diamonds, 6 of spades, 7 of clubs");
    Card **cpp = hp->cards;
    CU_ASSERT(hand_has_straight(hp));

    set_rank(cpp[3], "Q");
    CU_ASSERT_FALSE(hand_has_straight(hp));

    set_rank(cpp[3], "A");
    set_rank(cpp[4], "2");
    CU_ASSERT(hand_has_straight(hp));

    free_hand(hp);
}

void test_hands_tieing()
{
    Hand *hand1 = sample_hand();
    Hand *hand2 = sample_hand();
    CU_ASSERT(hand_tie(hand1, hand2));
}

void test_hands_winning()
{
    int i;
    Hand *hand1 = sample_hand();
    Hand *hand2 = sample_hand();
    Card **c1 = hand1->cards;
    Card **c2 = hand2->cards;
    set_rank(c2[2], "2");
//3 4 5 K 5
//3 4 2 K 5
    CU_ASSERT(hand_beats_hand(hand1, hand2));  // pair, nothing
    set_rank(c2[0], "2");
    set_rank(c2[4], "4");
// 3 4 5 K 5
// 2 4 5 K 4
    CU_ASSERT(hand_beats_hand(hand2, hand1));  // two pair, pair
    set_rank(c1[0], "5");
// 5 4 5 K 5
// 2 4 2 K 4
    CU_ASSERT(hand_beats_hand(hand1, hand2));  // 3-of-kind, two pair
    set_rank(c2[2], "3");
    set_rank(c2[3], "5");
    set_rank(c2[4], "6");
// 5 4 5 K 5
// 2 4 3 5 6
    CU_ASSERT(hand_beats_hand(hand2, hand1));  // straight, 3-of-kind
    for(i = 0; i < 5; i++) set_suit(c1[i], "clubs");
// all clubs in hand1
    CU_ASSERT(hand_beats_hand(hand1, hand2));  // flush, straight
// 5 4 5 K 5
// 2 4 3 5 6
    set_rank(c2[1], "2");
    set_rank(c2[3], "3");
    set_rank(c2[4], "3");
// 5 4 5 K 5
// 2 2 3 3 3
    CU_ASSERT(hand_beats_hand(hand2, hand1));  // full house, flush
    set_rank(c1[1], "5");
    set_suit(c1[1], "spades");
// 5 5 5 K 5
// 2 2 3 3 3
    CU_ASSERT(hand_beats_hand(hand1, hand2));  // four of a kind, full house
    for(i = 0; i < 5; i++) set_suit(c2[i], "clubs");
    set_rank(c2[1], "4");
    set_rank(c2[3], "5");
    set_rank(c2[4], "6");
// 5 5 5 K 5
// 2 4 3 5 6 of clubs
    CU_ASSERT(hand_beats_hand(hand2, hand1));  // straight flush, four of a kind
}

void test_high_pair_wins()
{
    Hand *hand1 = sample_hand();
    Hand *hand2 = sample_hand();
    set_rank(hand1->cards[2], "7");
    set_rank(hand1->cards[4], "7");
    CU_ASSERT(hand_beats_hand(hand1, hand2));
}

void test_high_card_wins_on_tied_pairs()
{
// 3 2 5 K 5 vs. 3 4 5 K 5
    Hand *hand1 = sample_hand();
    Hand *hand2 = sample_hand();
    set_rank(hand1->cards[1], "2");
    CU_ASSERT(hand_beats_hand(hand2, hand1));
// 3 A 5 K 5 vs. 3 4 5 K 5
    set_rank(hand1->cards[1], "A");
    CU_ASSERT(hand_beats_hand(hand1, hand2));
}

void test_high_trip_wins()
{
    Hand *hand1 = sample_hand();
    Hand *hand2 = sample_hand();
    set_rank(hand1->cards[3], "5");
    set_rank(hand2->cards[2], "3");
    set_rank(hand2->cards[4], "3");
    CU_ASSERT(hand_beats_hand(hand1, hand2));

}

void test_high_card_wins_on_tied_trips()
{
    Hand *hand1 = sample_hand();
    Hand *hand2 = sample_hand();
// 3 4 5 5 5 vs. Q 4 5 5 5
    set_rank(hand1->cards[3], "5");
    set_rank(hand2->cards[3], "5");
    set_rank(hand2->cards[0], "Q");
    CU_ASSERT(hand_beats_hand(hand2, hand1));

// 3 4 5 5 5 vs. 2 4 5 5 5
    set_rank(hand2->cards[0], "2");
    CU_ASSERT(hand_beats_hand(hand1, hand2));
}


void test_high_straight_wins()
{
    Hand *hand1 = sample_hand();
    Hand *hand2 = sample_hand();
    set_rank(hand1->cards[3], "6");
    set_rank(hand1->cards[4], "7");
    set_rank(hand2->cards[3], "6");
    set_rank(hand2->cards[4], "2");
    CU_ASSERT(hand_beats_hand(hand1, hand2));
}

void test_high_flush_wins() {
    Hand *hand1 = create_batch_hand("2 of clubs, 3 of clubs, 4 of clubs, 7 of clubs, 10 of clubs");
    Hand *hand2 = create_batch_hand("2 of spades, 3 of spades, 4 of spades, 7 of spades, J of spades");
    CU_ASSERT(hand_beats_hand(hand2, hand1));
}

void test_high_full_house_wins()
{
    Hand *hand1 = create_batch_hand("2 of clubs, 2 of diamonds, 2 of spades, 7 of clubs, 7 of hearts");
    Hand *hand2 = create_batch_hand("2 of clubs, 2 of diamonds, 7 of spades, 7 of clubs, 7 of hearts");
    CU_ASSERT(hand_beats_hand(hand2, hand1));
}

void test_tied_full_houses()
{
    Hand *hand1 = create_batch_hand("2 of clubs, 2 of diamonds, 2 of spades, 7 of clubs, 7 of hearts");
    Hand *hand2 = create_batch_hand("2 of clubs, 2 of diamonds, 2 of spades, 7 of clubs, 7 of hearts");
    CU_ASSERT(hand_tie(hand1, hand2));
}

void test_high_four_of_a_kind_wins()
{
    Hand *hand1 = create_batch_hand("2 of clubs, 2 of spades, 2 of hearts, 2 of diamonds, Q of hearts");
    Hand *hand2 = create_batch_hand("4 of clubs, 4 of spades, 4 of hearts, 4 of diamonds, Q of hearts");
    CU_ASSERT(hand_beats_hand(hand2, hand1));
}

void test_high_card_wins_on_tied_fours()
{
    Hand *hand1 = create_batch_hand("2 of clubs, 2 of spades, 2 of hearts, 2 of diamonds, Q of hearts");
    Hand *hand2 = create_batch_hand("2 of clubs, 2 of spades, 2 of hearts, 2 of diamonds, K of hearts");
    CU_ASSERT(hand_beats_hand(hand2, hand1));
}

void test_high_straight_flush_wins()
{
    Hand *hand1 = create_batch_hand("2 of clubs, 3 of clubs, 4 of clubs, 5 of clubs, 6 of clubs");
    Hand *hand2 = create_batch_hand("3 of spades, 4 of spades, 5 of spades, 6 of spades, 7 of spades");
    CU_ASSERT(hand_beats_hand(hand2, hand1));
}

void test_high_card_wins_with_nothing()
{
    Hand *hand1 = create_batch_hand("2 of clubs, 3 of clubs, 4 of clubs, 5 of clubs, 10 of spades");
    Hand *hand2 = create_batch_hand("2 of diamonds, 3 of diamonds, 4 of clubs, 5 of hearts, J of spades");
    CU_ASSERT(hand_beats_hand(hand2, hand1));
}

void test_high_two_pair_wins() {
    Hand *hand1 = create_batch_hand("2 of clubs, 2 of hearts, 4 of clubs, 4 of diamonds, 10 of spades");
    Hand *hand2 = create_batch_hand("6 of clubs, 6 of hearts, 4 of clubs, 4 of diamonds, 10 of clubs");
    CU_ASSERT(hand_beats_hand(hand2, hand1));
}

void test_high_card_wins_on_tied_two_pairs()
{
    Hand *hand1 = create_batch_hand("2 of clubs, 2 of hearts, 4 of clubs, 4 of diamonds, K of spades");
    Hand *hand2 = create_batch_hand("2 of clubs, 2 of hearts, 4 of clubs, 4 of diamonds, 10 of spades");
    CU_ASSERT(hand_beats_hand(hand1, hand2));
}

void test_pair_hash() {
    Hand *hand = sample_hand();
    set_rank(hand->cards[3], "2");
    Mult *pp = malloc(sizeof(Mult));
    int kickers[] = { 0, 1, 2 };
    n_of_a_kind_hash(hand, pp, 2);
    CU_ASSERT_EQUAL(pp->mult, 3);
    int *k = pp->kickers;
    CU_ASSERT(!memcmp(kickers, k, 3 * sizeof(int)));
}

int main()
{
    CU_BasicRunMode mode = CU_BRM_VERBOSE;
    CU_ErrorAction error_action = CUEA_IGNORE;

    CU_ErrorCode cue = CU_initialize_registry();
    if (cue != CUE_SUCCESS) {
	printf("ERROR in CU_initialize_registry\n");
	return;
    }
    CU_basic_set_mode(mode);
    CU_set_error_action(error_action);

    CU_pSuite cardBasics, handCreation, handRanking, handContents, handComparison;

    cardBasics = CU_add_suite("Card basics", my_suite_init, my_suite_clean);
    handCreation = CU_add_suite("Hand creation and initialization", my_suite_init, my_suite_clean);
    handRanking = CU_add_suite("Hand ranking routines", my_suite_init, my_suite_clean);
    handComparison = CU_add_suite("Hand comparison routines", my_suite_init, my_suite_clean);
    handContents = CU_add_suite("Hand content routines", my_suite_init, my_suite_clean);

    CU_ADD_TEST(cardBasics, test_pretty_formatting);
    CU_ADD_TEST(cardBasics, test_card_comparison);
    CU_ADD_TEST(cardBasics, test_card_gteqlt);
    CU_ADD_TEST(cardBasics, test_rank_difference);

    CU_ADD_TEST(handCreation, test_create_hand_with_multiple_specs);
    CU_ADD_TEST(handCreation, test_add_card_to_hand);

    CU_ADD_TEST(handRanking, test_rank_index);
    CU_ADD_TEST(handRanking, test_reporting_rank_of_hand);

    CU_ADD_TEST(handContents, test_n_of_a_kind);
    CU_ADD_TEST(handContents, test_straights);
    CU_ADD_TEST(handContents, test_flush);
    CU_ADD_TEST(handContents, test_straight_flush);

    CU_ADD_TEST(handComparison, test_hands_tieing);
    CU_ADD_TEST(handComparison, test_hands_winning);
    CU_ADD_TEST(handComparison, test_high_pair_wins);
    CU_ADD_TEST(handComparison, test_high_card_wins_on_tied_pairs);
    CU_ADD_TEST(handComparison, test_high_two_pair_wins);
    CU_ADD_TEST(handComparison, test_high_trip_wins);
    CU_ADD_TEST(handComparison, test_high_card_wins_on_tied_trips);
    CU_ADD_TEST(handComparison, test_high_straight_wins);
    CU_ADD_TEST(handComparison, test_high_flush_wins);
    CU_ADD_TEST(handComparison, test_high_full_house_wins);
    CU_ADD_TEST(handComparison, test_tied_full_houses);
    CU_ADD_TEST(handComparison, test_high_four_of_a_kind_wins);
    CU_ADD_TEST(handComparison, test_high_card_wins_on_tied_fours);
    CU_ADD_TEST(handComparison, test_high_straight_flush_wins);
   CU_ADD_TEST(handComparison, test_pair_hash);
    CU_ADD_TEST(handComparison, test_high_card_wins_on_tied_two_pairs);
    
    CU_basic_run_tests();
    CU_cleanup_registry();
}
