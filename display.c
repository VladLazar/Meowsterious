#include "display.h"

uint16_t text_box_y = 0;

void write_line_to_text_box(const char* text, uint16_t col);

void initialize_display() {
    /* 8MHz clock, no prescaling (DS, p. 48) */
    CLKPR = (1 << CLKPCE);
    CLKPR = 0;

    init_lcd();
}

void draw_game_map(game_map* map) {
    // If the map cannot fit on the display it does not get drawn.
    if (MAX_X * (FONT_WIDTH + GAME_MAP_X_GAP) > GAME_MAP_X_MAX
        || MAX_Y * (FONT_HEIGHT + GAME_MAP_Y_GAP) > GAME_MAP_Y_MAX)
        return;

    size_t z = map->player.z;

    for (size_t y = MIN_Y; y <= MAX_Y; ++y)
        for (size_t x = MIN_X; x <= MAX_X; ++x) {
            char to_display = map->player.x == x && map->player.y == y ?
                              '@' : map->matrix[z][y][x];

            uint16_t x_display = x * (FONT_WIDTH + GAME_MAP_X_GAP);
            uint16_t y_display = y * (FONT_HEIGHT + GAME_MAP_Y_GAP);

            display_char_xy(to_display, x_display, y_display, WHITE);
        }
}

void clear_game_map() {
    rectangle r;
    r.left = GAME_MAP_X_MIN;
    r.right = GAME_MAP_X_MAX;
    r.top = GAME_MAP_Y_MIN;
    r.bottom = GAME_MAP_Y_MAX;

    fill_rectangle(r, BLACK);
}

void write_to_text_box(const char* text, uint16_t col) {
    size_t length = strlen(text);

    /* Break the string up into lines that can fit. */
    for (size_t i = 0; i < length; i += TEXT_BOX_LINE_LENGTH) {
        size_t line_length = TEXT_BOX_LINE_LENGTH > (length - i + 1)
                             ? (length - i + 1) : TEXT_BOX_LINE_LENGTH;
        char line[TEXT_BOX_LINE_LENGTH];
        strncpy(line, text + i, line_length);

        write_line_to_text_box(line, col);
    }
}

void write_line_to_text_box(const char* text, uint16_t col) {
    display_string_xy(text, TEXT_BOX_X_MIN, text_box_y, col);
    text_box_y += FONT_HEIGHT + 5;
}

void clear_text_box() {
    rectangle r;
    r.left = TEXT_BOX_X_MIN;
    r.right = TEXT_BOX_X_MAX;
    r.top = TEXT_BOX_Y_MIN;
    r.bottom = TEXT_BOX_Y_MAX;

    fill_rectangle(r, BLACK);
    text_box_y = 0;
}

void write_interaction(interaction* current_interaction,
        const char* top_line, uint8_t selected_index, uint8_t show_options) {
    if (current_interaction == NULL
        || (selected_index >= current_interaction->size_options
            && selected_index != NONE_SELECTED))
        return;

    if (selected_index != NONE_SELECTED)
         current_interaction->showing = selected_index;

    strcpy(current_interaction->current_line, top_line);
    write_to_text_box(top_line, WHITE);

    if (!show_options)
        return;

    for (size_t i = 0; i < current_interaction->size_options; ++i) {
        uint16_t col = i == selected_index ? YELLOW : WHITE;

        /*
         * Allocate a buffer large enough to fit the text.
         * Note that we also need to take into account the
         * number appended at the front.
         */
        char line[MAX_LINE_SIZE];
        get_player_line(line, current_interaction, i);
        size_t buffer_size = strlen(line) + 4;
        char* to_write = malloc(buffer_size * sizeof(char));

        snprintf(to_write, buffer_size, "%u. %s", i, line);
        write_to_text_box(to_write, col);

        free(to_write);
    }
}
