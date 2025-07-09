// header per blackjackclient.c
// contiene dichiarazioni delle funzioni da usare in altri file e definizione di enum
// i valori dati agli enum non hanno significato, sono completamente arbitrari


typedef enum login_mode
{
    LOGIN_MODE  = 20,
    REGISTER_MODE
} MODE;

typedef enum login_result
{
    LOGIN_SUCCESS = 2,
    LOGIN_FAIL,
    REGISTER_SUCCESS,
    REGISTER_FAIL
} RESULT;

int server_login(char username[], char password[], MODE mode);
int money_value_update(int modify_value);
int money_value_get();
