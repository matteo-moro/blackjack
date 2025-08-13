typedef struct card
{
	int rank;
	char* suit;
} CARD;

typedef struct deck
{
	CARD* cards;
	int current_card_number;
} DECK;
