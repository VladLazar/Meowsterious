#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lcd.h"
#include "game_map.h"
#include "interaction.h"

/*
 * The screen shall be split in two "boxes", one to display the game map
 * and the other one to display any text needed. The coordinates of the
 * rectangles defining them can be found below.
 */

#define GAME_MAP_X_MIN 0
#define GAME_MAP_Y_MIN 0
#define GAME_MAP_X_MAX (LCDHEIGHT / 2 - 5)
#define GAME_MAP_Y_MAX LCDWIDTH

#define GAME_MAP_X_GAP 3
#define GAME_MAP_Y_GAP 5

#define TEXT_BOX_X_MIN (LCDHEIGHT / 2 + 5)
#define TEXT_BOX_Y_MIN 0
#define TEXT_BOX_X_MAX LCDHEIGHT
#define TEXT_BOX_Y_MAX LCDWIDTH
#define TEXT_BOX_LINE_LENGTH (TEXT_BOX_X_MAX - TEXT_BOX_X_MIN) / (FONT_WIDTH - TEXT_OVERLAP)

extern uint16_t text_box_y;

void initialize_display();

void draw_game_map(game_map* map);
void clear_game_map();

void write_to_text_box(const char* string, uint16_t col);
void clear_text_box();

void write_interaction(interaction* current_interaction, const char* top_line, 
        uint8_t selected_answer, uint8_t show_options);

#endif /* DISPLAY_H */
