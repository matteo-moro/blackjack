//completamente WIP, va separato in .c e .h

#ifndef STRUCTS_INCLUDE
#define STRUCTS_INCLUDE
#include "bj-structs.h"
#include "cardlib.h"
#endif

#ifndef RAY_INCLUDE
#define RAY_INCLUDE
#include <raylib.h>
#include <raygui.h>
#endif

#ifndef STRING_INCLUDE
#define STRING_INCLUDE
#include <string.h>
#endif

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

Player player_init(Game* game)
{
	Player player;
	for (int i = 0; i < STARTING_HAND_SIZE; i++)
	{
		player->Hand[i] = draw_card(&game->Deck);
	}
	player->card_number = STARTING_HAND_SIZE;
	player->hand_score = calculate_score(player->Hand, player->card_number);
	player->money_value = player->money_bet = 0;
	return player;
}

Dealer dealer_init(Game* game)
{
	Dealer dealer;
	for (int i = 0; i < STARTING_HAND_SIZE; i++)
	{
		dealer->Hand[i] = draw_card(&game->Deck);
	}
	dealer->card_number = STARTING_HAND_SIZE;
	dealer->hand_score = calculate_score(dealer->Hand, dealer->card_number);
}

Game game_init()
{
	Game game;
	game->Deck = fill_deck(NUMBER_OF_DECKS);
	shuffle_deck(&game->Deck);
	strcpy(game->money_value_string, "");
	strcpy(game->money_bet_string, "");
	game->CardFont = LoadFontEx("JqkasWild-w1YD6.tff", 128, 0, 250);
	game->SuitFont = LoadFontEx("JqkasWild-w1YD6.tff", 32, 0, 250);
	game->DefaultFont = GetFontDefault();
	game->screen_width = (float)GetScreenWidth();
	game->screen_height = (float)GetScreenHeight();
	for (int i = 0; i < MAX_HAND_SIZE; i++)
	{
		game->PlayerCardRecArray[i].x = game->DealerCardRecArray[i].x = (0.03f + (i * 0.171f)) * game->screen_width;
		game->PlayerCardRecArray[i].width = game->DealerCardRecArray[i].width = game->screen_width * 0.156f;
		game->PlayerCardRecArray[i].y = game->screen_height * 0.55f;
		game->DealerCardRecArray[i].y = game->screen_height * 0.1f;
		game->PlayerCardRecArray[i].height = game->DealerCardRecArray[i].height = screen_height * 0.35f;
	}


}
