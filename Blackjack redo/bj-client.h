#ifndef BOOL_INCLUDE
#define BOOL_INCLUDE
#include <stdbool.h>
#endif

#ifndef STRUCTS_INCLUDE
#define STRUCTS_INCLUDE
#include "bj-structs.h"
#endif

bool server_login(char username[], char password[], MODE mode);
bool money_value_update(int modify_value);
int money_value_get();