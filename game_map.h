#ifndef GAME_MAP_H
#define GAME_MAP_H

#include <stdint.h>
#include <string.h>
#include <stddef.h>

#define MIN_X 0
#define MAX_X 7

#define MIN_Y 0
#define MAX_Y 17

#define MIN_Z 0
#define MAX_Z 1

#define MAX_ITEMS        3
#define MAX_ITEM_NAME    10

typedef struct position {
    uint8_t x;
    uint8_t y;
    uint8_t z;
} position;

typedef struct game_map {
    char matrix[MAX_Z + 1][MAX_Y + 1][MAX_X + 1];
    position player;
} game_map;

typedef struct move_to {
    uint8_t allowed;
    char on;
} move_to;


typedef enum {move_north, move_west, move_south, move_east, move_down, move_up} direction;

extern uint8_t items_size;
extern char items[MAX_ITEMS][MAX_ITEM_NAME];

uint8_t has_item(const char* item_name);
void add_item(const char* item_name);

move_to move_player(game_map* map, direction dir);


#endif /* GAME_MAP_H */
