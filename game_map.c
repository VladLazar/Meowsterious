#include "game_map.h"

char items[MAX_ITEMS][MAX_ITEM_NAME];

uint8_t has_item(const char* item_name) {
    for (size_t i = 0; i < MAX_ITEMS; ++i)
        if (strcmp(items[i], item_name) == 0)
            return 1;

    return 0;
}

void add_item(const char* item_name) {
    if (items_size == MAX_ITEMS)
        return;

    strncpy(items[++items_size], item_name, MAX_ITEM_NAME);
}

move_to move_player_north(game_map* map) {
    uint8_t current_x = map->player.x;
    uint8_t current_y = map->player.y;
    uint8_t current_z = map->player.z;

    char next_char = map->matrix[current_z][current_y - 1][current_x];

    if (current_y == MIN_Y || next_char == '#' 
        || (next_char == '=' && !has_item("key")))
        return (move_to) {0, next_char};

    map->player.y--;
    return (move_to) {1, next_char};
}

move_to move_player_east(game_map* map) {
    uint8_t current_x = map->player.x;
    uint8_t current_y = map->player.y;
    uint8_t current_z = map->player.z;

    char next_char = map->matrix[current_z][current_y][current_x + 1];

    if (current_x == MAX_X || next_char == '#'
        || (next_char == '=' && !has_item("key")))
        return (move_to) {0, next_char};

    map->player.x++;
    return (move_to) {1, next_char};
}

move_to move_player_south(game_map* map) {
    uint8_t current_x = map->player.x;
    uint8_t current_y = map->player.y;
    uint8_t current_z = map->player.z;

    char next_char = map->matrix[current_z][current_y + 1][current_x];

    if (current_y == MAX_Y || next_char == '#'
        || (next_char == '=' && !has_item("key")))
        return (move_to) {0, next_char};

    map->player.y++;
    return (move_to) {1, next_char};
}

move_to move_player_west(game_map* map) {
    uint8_t current_x = map->player.x;
    uint8_t current_y = map->player.y;
    uint8_t current_z = map->player.z;

    char next_char = map->matrix[current_z][current_y][current_x - 1];

    if (current_x == MIN_X || next_char == '#'
        || (next_char == '=' && !has_item("key")))
        return (move_to) {0, next_char};

    map->player.x--;
    return (move_to) {1, next_char};
}

move_to move_player_up(game_map* map) {
    uint8_t current_x = map->player.x;
    uint8_t current_y = map->player.y;
    uint8_t current_z = map->player.z;

    if (current_z == MAX_Z)
        return (move_to) {0, '#'};

    map->player.z++;
    return (move_to) {1, map->matrix[current_z + 1][current_y][current_x]};
}

move_to move_player_down(game_map* map) {
    uint8_t current_x = map->player.x;
    uint8_t current_y = map->player.y;
    uint8_t current_z = map->player.z;

    if (current_z == MIN_Z)
        return (move_to) {0, '#'};

    map->player.z--;
    return (move_to) {1, map->matrix[current_z - 1][current_y][current_x]};
}

move_to move_player(game_map* map, direction dir) {
    switch(dir) {
        case move_north: return move_player_north(map);
        case move_south: return move_player_south(map);
        case move_east: return move_player_east(map);
        case move_west: return move_player_west(map);
        case move_down: return move_player_down(map);
        case move_up: return move_player_up(map);
    }
}
