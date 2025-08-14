#include <raylib.h>
#include <raygui.h>
#include "bj-structs.h"
#include "cardlib.h"
#include <string.h>
#include "bj-gamewindow-structs.h"

Player player_init(Game* game)
{
	Player player;
	for (int i = 0; i < STARTING_HAND_SIZE; i++)
	{
		player.Hand[i] = draw_card(&game->Deck);
	}
	player.card_number = STARTING_HAND_SIZE;
	player.hand_score = calculate_score(player.Hand, player.card_number);
	player.money_value = player.money_bet = 0;
	return player;
}

Dealer dealer_init(Game* game)
{
	Dealer dealer;
	for (int i = 0; i < STARTING_HAND_SIZE; i++)
	{
		dealer.Hand[i] = draw_card(&game->Deck);
	}
	dealer.card_number = STARTING_HAND_SIZE;
	dealer.hand_score = calculate_score(dealer.Hand, dealer.card_number);
}

Game game_init()
{
	Game game;

	game.Deck = fill_deck(NUMBER_OF_DECKS);
	shuffle_deck(&game.Deck);

	strcpy(game.money_value_string, "");
	strcpy(game.money_bet_string, "");
	game.CardFont = LoadFontEx("JqkasWild-w1YD6.tff", 128, 0, 250);
	game.SuitFont = LoadFontEx("JqkasWild-w1YD6.tff", 32, 0, 250);
	game.DefaultFont = GetFontDefault();

	game.screen_width = (float)GetScreenWidth();
	game.screen_height = (float)GetScreenHeight();
	for (int i = 0; i < MAX_HAND_SIZE; i++)
	{
		game.PlayerCardRecArray[i].x = game.DealerCardRecArray[i].x = (0.03f + (i * 0.171f)) * game.screen_width;
		game.PlayerCardRecArray[i].width = game.DealerCardRecArray[i].width = game.screen_width * 0.156f;
		game.PlayerCardRecArray[i].y = game.screen_height * 0.55f;
		game.DealerCardRecArray[i].y = game.screen_height * 0.1f;
		game.PlayerCardRecArray[i].height = game.DealerCardRecArray[i].height = game.screen_height * 0.35f;
	}
	game.ButtonAreaRec.x = game.screen_width * 0.9f;
	game.ButtonAreaRec.y = 0;
	game.ButtonAreaRec.width = game.screen_width * 0.1f;
	game.ButtonAreaRec.height = game.screen_height;

	for (int i = 0; i < BUTTON_NUMBER; i++)
	{
		game.ButtonArray[i].x = game.screen_width * 0.91f;
		game.ButtonArray[i].y = (0.01f + (i * 0.07f)) * game.screen_height;
		game.ButtonArray[i].width = game.screen_width * 0.08f;
		game.ButtonArray[i].height = game.screen_height * 0.05f;
	}

	game.MoneyTextVec.x = game.MoneyBetTextVec.x = game.screen_width * 0.03f;
	game.MoneyTextVec.y = game.screen_height * 0.47f;
	game.MoneyBetTextVec.y = game.screen_height * 0.5f;

}
