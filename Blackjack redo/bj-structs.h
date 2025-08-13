typedef enum login_mode
{
    LOGIN_MODE = 20,
    REGISTER_MODE
} MODE;

typedef enum endgame_signals
{
    PLAYER_WIN = 100,
    PLAYER_LOSE,
    PLAYER_BUST,
    PLAYER_CHARLIE,
    PLAYER_BLACKJACK,
    PLAYER_DRAW,
    DEALER_BUST,
    DEALER_BLACKJACK
} SIGNAL;
