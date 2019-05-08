#ifndef INTERACTION_H
#define INTERACTION_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "game_map.h"
#include "OSFS.h"

#define WIN_CODE 42u

#define MAX_INTERACTIONS 6
#define MAX_LINE_SIZE    80
#define MAX_OPTIONS      4
#define NONE_SELECTED    32
#define MAX_NAME_SIZE    10

typedef enum {npc, scene} interaction_type;

typedef struct text {
    char player_file[MAX_NAME_SIZE];
    char world_file[MAX_NAME_SIZE];
} text;

typedef struct interaction {
    interaction_type type;

    char name[MAX_NAME_SIZE - 3];
    char on_map;
    position pos;

    char greet_file[MAX_NAME_SIZE];
    char current_line[MAX_LINE_SIZE];

    size_t size_options;
    uint8_t showing;
    text options[MAX_OPTIONS];

    uint8_t (*on_select_option)(game_map* map, uint8_t selected_index);
} interaction;

extern interaction interactions[MAX_INTERACTIONS];

void initialize_interactions();
interaction* find_interaction_by_char(const char c);
interaction* find_interaction_by_pos(position pos);
interaction* find_interaction_by_name(const char* name);

uint16_t add_line(interaction* inter, const char* player_line, const char* world_line);
uint16_t remove_line(interaction* inter, const uint8_t index);

void get_player_line(char* buf, interaction* inter, size_t index);
void get_world_line(char* buf, interaction* inter, size_t index);
void get_greet_line(char* buf, interaction* inter);

#endif /* INTERACTION_H */
