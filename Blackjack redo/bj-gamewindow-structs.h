#include "cardlib.h"
#include <raylib.h>

#define MAX_HAND_SIZE 5
#define DEFAULT_BUFLEN 20
#define BUTTON_NUMBER 5
#define STARTING_HAND_SIZE 2
#define NUMBER_OF_DECKS 8

typedef struct player
{
	CARD Hand[MAX_HAND_SIZE];
	int card_number;
	int hand_score;
	int money_value;
	int money_bet;
} Player;

typedef struct dealer
{
	CARD Hand[MAX_HAND_SIZE];
	int card_number;
	int hand_score;
} Dealer;

typedef struct game
{
	DECK Deck;

	char money_value_string[DEFAULT_BUFLEN]
	char money_bet_string[DEFAULT_BUFLEN];
	float screen_width;
	float screen_height;
	Font CardFont;
	Font SuitFont;
	Font DefaultFont;
	Rectangle PlayerCardRecArray[MAX_HAND_SIZE];
	Rectangle DealerCardRecArray[MAX_HAND_SIZE];
	Rectangle ButtonArray[BUTTON_NUMBER];
	Rectangle ButtonAreaRec;
	Vector2 MoneyTextVec;
	Vector2 MoneyBetTextVec;
} Game;

Player player_init(Game* game);
Dealer dealer_init(Game* game);
Game game_init();
