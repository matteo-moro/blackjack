#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include "bj-client.h"
#include "bj-gamewindow.h"
#include "bj-gamemenu.h"
#include "bj-structs.h"

#ifndef STRUCTS_INCLUDE
#define STRUCTS_INCLUDE
#include "bj-structs.h"
#endif

#ifndef BOOL_INCLUDE
#define BOOL_INCLUDE
#include <stdbool.h>
#endif

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

int main(void)
{
    InitWindow(1200, 625, "Blackjack");
    SetTargetFPS(60);

    ONMODE online_play = MainMenu();
    bool retry = false;

    if (online_play == ERR_VALUE)
    {
        exit(1);
    }
    else
    {
        do
        {
            retry = GameStart(online_play);
        } while (retry == true);
    }

    return 0;
}