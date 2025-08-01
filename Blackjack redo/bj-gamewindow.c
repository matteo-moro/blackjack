#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <raylib.h>
#include <raygui.h>
#include "cardlib.h"
#include "bj-client.h"
#include "bj-gamewindow.h"

#ifndef STRUCTS_INCLUDE
#def STRUCTS_INCLUDE
#include "bj-structs.h"
#endif

#ifndef BOOL_INCLUDE
#def BOOL_INCLUDE
#include <stdbool.h>
#endif

#define BUTTON_NUMBER 5 //numero di bottoni nell'UI, necessario per funzionamento corretto dell'interfaccia
#define MAX_HAND_SIZE 5 
#define NUMBER_OF_DECKS 8 //numero di mazzi usati per costruire il mazzo di gioco
#define STARTING_HAND_SIZE 2
#define BLACKJACK_SCORE 21
#define MIN_STAND_SCORE 17 //punteggio dal quale il dealer è obbligato a fare stand
#define DEFAULT_BUFLEN 20
// il font delle carte usa questi simboli per mostrare il seme
#define CLUBS "}"
#define DIAMONDS "{"
#define HEARTS "<"
#define SPADES ">"

CARD PlayerHand[MAX_HAND_SIZE] = {0};
CARD DealerHand[MAX_HAND_SIZE] = {0};
DECK GameDeck;
SIGNAL endgame_code;
int player_card_num, player_hand_score, dealer_card_num, dealer_hand_score, money_value, money_bet, rel_x, rel_y;
char money_value_string[DEFAULT_BUFLEN];
char money_bet_string[DEFAULT_BUFLEN];
float screen_width = 0, screen_height = 0;
Font CardFont, SuitFont, DefaultFont;
Rectangle PlayerCardRecArray[MAX_HAND_SIZE];
Rectangle DealerCardRecArray[MAX_HAND_SIZE];
Rectangle ButtonArray[BUTTON_NUMBER]; // i bottoni sono in un array per facilitare il controllo delle collisioni
Rectangle ButtonAreaBGRec;
Vector2 MoneyTextVec, MoneyBetTextVec;
bool hit_button_pressed, stand_button_pressed, surrent_button_pressed, doubledown_button_pressed, bet_button_pressed, end_game,
    retry, set_bet, updated_money, button_collision, sizes_changed, end_screen_changed, bet_screen_changed;

void GameWindow_init(float gw_screen_width, gw_screen_height)
{
    sizes_changed = false;
    GameDeck = fill_deck(NUMBER_OF_DECKS);
    shuffle_deck(&GameDeck);

    for (int i = 0; i < STARTING_HAND_SIZE; i++)
    {
        PlayerHand[i] = draw_card(&GameDeck);
        DealerHand[i] = draw_card(&GameDeck);
    }

    player_card_num = dealer_card_num = STARTING_HAND_SIZE;
    player_hand_score = calculate_score(PlayerHand, player_card_num);
    dealer_hand_score = calculate_score(DealerHand, dealer_card_num);

    money_value = money_bet = endgame_code = 0;

    strcpy(money_value_string, "");
    strcpy(money_bet_string, "");

    end_game = retry = set_bet = updated_money = hit_button_pressed =
        stand_button_pressed = surrent_button_pressed = doubledown_button_pressed =
        bet_button_pressed = button_collision = end_screem_changed = bet_screen_changed = false;

    CardFont = LoadFontEx("JqkasWild-w1YD6.ttf", 128, 0, 250);
    SuitFont = LoadFontEx("JqkasWild-w1YD6.tff", 32, 0, 250);
    DefaultFont = GetFontDefault();

    if (gw_screen_width != screen_width)
    {
        screen_width = gw_screen_width;

        //! tutti i numeri sono arbitrati per il posizionamento e le proporzioni corrette
        
        for (int i = 0; i < MAX_HAND_SIZE; i++)
        {
            PlayerCardRecArray[i].x = DealerCardRecArray[i].x = (0.03f + (i * 0.171f)) * screen_width;
            PlayerCardRecArray[i].width = DealerCardRecArray[i].width = screen_width * 0.156f;
        }

        ButtonAreaBGRec.x = screen_width * 0.9f;
        ButtonAreaBGRec.width = screen_width * 0.1f;

        for (int i = 0; i < BUTTON_NUMBER; i++)
        {
            ButtonArray[i].x = screen_width * 0.91f;
            ButtonArray[i].width = screen_width * 0.08f;
        }

        MoneyTextVec.x = MoneyBetTextVec.x = screen_width * 0.03f;

        sizes_changed = true;
    }

    if (gw_screen_height != screen_height)
    {
        screen_height = gw_screen_height;

        for (int i = 0; i < MAX_HAND_SIZE; i++)
        {
            PlayerCardRecArray[i].y = screen_height * 0.55f;
            DealerCardRecArray[i].y = screen_height * 0.1f;
            PlayerCardRecArray[i].height = DealerCardRecArray[i].height = screen_height * 0.35f;
        }

        ButtonAreaBGRec.y = 0;
        ButtonAreaBGRec.height = screen_height;

        for (int i = 0; i < BUTTON_NUMBER; i++)
        {
            ButtonArray[i].y = (0.01f + (i * 0.07f)) * screen_height;
            ButtonArray[i].height = screen_height * 0.05f;
        }

        MoneyTextVec.y = screen_height * 0.47f;
        MoneyBetTextVec.y = screen_height * 0.5f;

        sizes_changed = true;
    }

    if (sizes_changed)
    {
        // implementazione di formula per trovare un punto sulla diagonale di un rettangolo dati altezza e lunghezza
        // pensando al segmento di diagonale delimitato dal punto da trovare come alla diagonale di un rettangolo più piccolo,
        // il rapporto fra le misure del rettangolo rimarrà uguale a quello maggiore.
        // sostituendo a x il valore y * ratio si ottiene che la diagonale è uguale alla radice quadrata di y^2 * (1 + ratio^2).
        // prendendo il quadrato della formula e dividendo per (1 + ratio^2) ottengo y^2.
        // Da y^2 poi posso ottenere y e di conseguenza x con ulteriori passaggi.
        float width = PlayerCardRecArray[0].width;
        float height = PlayerCardRecArray[0].height;
        float card_ratio = width / height;
        float temp_formula_value = 1 + (card_ratio * card_ratio);
        float diagonal_size = sqrtf((width * width) + (height * height));
        float diag_seg_size = diagonal_size * 0.1f;
        rel_y = sqrtf((diag_seg_size * diag_seg_size) / temp_formula_value);
        rel_x = rel_y * card_ratio;
    }
}

bool GameStart(bool is_online)
{
    if (!IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE))
    {
        SetWindowState(FLAG_BORDERLESS_WINDOWED_MODE);
    }
    GuiSetStyle(DEFAULT, TEXT_SIZE, 28);
    GuiSetStyle(DEFAULT, TEXT_SPACING, 1);

    float gw_screen_width = (float)GetScreenWidth();
    float gw_screen_height = (float)GetScreenHeight();

    GameWindow_init(gw_screen_width, gw_screen_height);

    if (is_online)
    {
        money_value = money_value_get();
    }

    while (!WindowShouldClose())
    {
        if (!end_game && !set_bet)
        {
            if (hit_button_pressed && player_card_num < MAX_HAND_SIZE)
            {
                hit_button_pressed = false;
                player_card_num += 1;
                PlayerHand[player_card_num - 1] = draw_card(&GameDeck);
                player_hand_score = calculate_score(PlayerHand, player_card_num);

            }

            if (stand_button_pressed || doubledown_button_pressed)
            {
                if (doubledown_button_pressed)
                {
                    player_card_num += 1;
                    PlayerHand[player_card_num - 1] = draw_card(&GameDeck);
                    player_hand_score = calculate_score(PlayerHand. player_card_num);
                    money_bet *= 2;
                }

                while (dealer_hand_score < MIN_STAND_SCORE && dealer_card_num < MAX_HAND_SIZE)
                {
                    dealer_card_num += 1;
                    DealerHand[dealer_card_num - 1] = draw_card(&GameDeck);
                    dealer_hand_score = calculate_score(DealerHand, dealer_card_num);
                }

                end_game = true;
            }

            if (surrent_button_pressed)
            {
                // valori impostati per garantire sconfitta per il giocatore
                player_hand_score = 0;
                dealer_hand_score = 21;
                money_bet /= 2;
                end_game = true;
            }

            if (bet_button_pressed && is_online)
            {
                set_bet = true;
            }
        }

        BeginDrawing();

        ClearBackground(DARKGREEN);

        for (int i = 0; i < player_card_num; i++)
        {
            window_draw_card(PlayerHand[i], PlayerCardRecArray[i]);
        }

        for (int i = player_card_number; i < MAX_HAND_SIZE; i++)
        {
            DrawRectangleRec(PlayerCardRecArray[i], GREEN);
        }

        if (dealer_card_num == STARTING_HAND_SIZE && !stand_button_pressed && !end_game)
        {
            for (int i = 0; i < STARTING_HAND_SIZE - 1; i++)
            {
                window_draw_card(DealerHand[i], DealerCardRecArray[i]);
            }

            DrawRectangleRec(DealerCardRecArray[STARTING_HAND_SIZE - 1], DARKBLUE);

            for (int i = STARTING_HAND_SIZE; i < MAX_HAND_SIZE; i++)
            {
                DrawRectangleRec(DealerCardRecArray[i], GREEN);
            }
        }
        else
        {
            for (int i = 0; i < dealer_card_num; i++)
            {
                window_draw_card(DealerHand[i], DealerCardRecArray[i]);
            }

            for (int i = dealer_card_num; i < MAX_CARD_NUM; i++)
            {
                DrawRectangleRec(DealerCardRecArray[i], GREEN);
            }
        }

        DrawRectangleRec(ButtonAreaBGRec, BROWN);
        // i rettangoli per i bottoni sono contenuti in un array per facilitare il controlli delle collisioni
        hit_button_pressed = GuiButton(ButtonArray[0], "HIT!")
        stand_button_pressed = GuiButton(ButtonArray[1], "STAND");
        surrend_button_pressed = GuiButton(ButtonArray[2], "SURREND");
        doubledown_button_pressed = GuiButton(ButtonArray[3], "DOUBLE!");
        bet_button_pressed = GuiButton(ButtonArray[4], "BET!");
    }
}