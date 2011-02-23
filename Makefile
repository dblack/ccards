tests:	test/cards.c
	gcc -o test/cards cards.c hand.c hand-comp.c profile.c test/cards.c -L/usr/local/lib -lcunit

test:	tests
	test/cards
