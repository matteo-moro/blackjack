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
