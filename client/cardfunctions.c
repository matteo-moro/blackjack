// codice per la libreria cardfunctions usata per la gestione di mazzi e carte dei tipi specificati in cardstructs
// la libreria è pensata per essere riutilizzabile fuori dal progetto.
// alcune funzioni dedicate al blackjack sono inserite, per essere usate sarà necessario definire GAME_BLACKJACK

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cardstructs.h"

void card_swap(CARD* first, CARD* second);

// crea un oggetto deck con numero di carte basato sul valore in input
// il deck creato viene ritornato alla fine della funzione
// il deck creato viene riempito di carte
// lo spazio in memoria che contiene le carte è allocato con malloc, ricordarsi di usare free
DECK fill_deck(int deck_number)
{
    int rank, suit_number;
    
    DECK created_deck = { 0 };
    CARD card_to_add = { 0 };

    created_deck.cards = malloc(52 * deck_number * sizeof(CARD));

    if (created_deck.cards == NULL)
    {
        printf("No memory to write data. Program closing.\n");
        exit(1);
    }

    for (int i = 0; i < deck_number; i++)
    {
        for (int j = 0; j < 52; j++)
        {
            suit_number = j / 13; // 13 carte per ogni seme, quindi divido per 13
            rank = j + 1; // correzione per la partenza da 0

            // gestione valori oltre 13 per riportare a valori corretti nelle carte
            while (rank > 13)
            {
                rank -= 13;
            }

            card_to_add.rank = rank;

            switch (suit_number)
            {
            case 0:
                card_to_add.suit = "Clubs";
                break;
            case 1:
                card_to_add.suit = "Diamonds";
                break;
            case 2:
                card_to_add.suit = "Hearts";
                break;
            case 3:
                card_to_add.suit = "Spades";
                break;
            }

            created_deck.cards[j + (i * 52)] = card_to_add;
        }
    }

    created_deck.current_card_number = deck_number * 52;

    return created_deck;
                    
}

// mischia un mazzo passato tramite puntatore attraverso lo shuffle di Fisher-Yates
void shuffle_deck(DECK* to_shuffle)
{
    stand(time(NULL));

    int card_number = to_shuffle->current_card_number;

    for (int i = (card_number - 1); i > 0; i--)
    {
        int j = rand() % (i+1);
        card_swap(&to_shuffle->cards[i], &to_shuffle->cards[j]);
    }
}

// funzione util per scambiare carte
void card_swap(CARD* first, CARD* second)
{
    CARD temp = *first:
    *first = *second;
    *second = temp;
}

// pesca una carta dal mazzo dato e la ritorna
CARD draw_card(DECK* deck_to_use)
{
    CARD card_to_return = { 0 };

    card_to_return.rank = deck_to_use->cards[0].rank;
    card_to_return.suit = deck_to_use->cards[0].suit;

    int card_number = deck_to_use->current_card_number;

    for (int i = 0; i < card_number - 1; i++)
    {
        deck_to_use->cards[i].rank = deck_to_use->cards[i + 1].rank;
        deck_to_use->cards[i].suit = deck_to_use->cards[i + 1].suit;
    }

    return card_to_return;
}

#ifdef GAME_BLACKJACK

int bj_calculate_score(CARD hand_to_evaluate[], int card_number)
{
    int score_to_return = 0;

    for (int i = 0; i < card_number; i++)
    {
        // visto che gli assi possono essere 11 o 1, sono prima trattati come 11 e poi eventualmente portati ad 1
        if (hand_to_evaluate[i].rank == 1)
        {
            score_to_return += 11;
        }
        //le figure valgono sempre 10
        else if (hand_to_evaluate[i].rank > 10)
        {
            score_to_return += 10;
        }
        else
        {
            score_to_return += hand_to_evaluate[i].rank;
        }
    }
}

#endif
