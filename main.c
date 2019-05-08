#include <stdio.h>
#include <avr/interrupt.h>
#include "display.h"
#include "game_map.h"
#include "rios.h"
#include "input.h"
#include "interaction.h"
#include "OSFS.h"

#define ON_NPC   1
#define ON_SCENE 2

uint8_t in_interaction = 0;

game_map map = { .matrix = {{"########",
                             "#......#",
                             "#......#",
                             "#....G.#",
                             "###..###",
                             "#......#",
                             "#......#",
                             "#......#",
                             "#......#",
                             "#......#",
                             "#......#",
                             "#......#",
                             "#......#",
                             "#.?....#",
                             "#......#",
                             "#......#",
                             "#.....?#",
                             "########"},
                            {"########",
                             "#?.....#",
                             "#...C..#",
                             "#......#",
                             "#......#",
                             "######=#",
                             "#......#",
                             "#......#",
                             "#......#",
                             "#......#",
                             "#......#",
                             "#......#",
                             "#......#",
                             "#......#",
                             "#......#",
                             "#......#",
                             "#.....?#",
                             "########"}},
                 .player = {1, 1, 0} };


int check_switches(int);
int collect_delta(int);
void on_switch(direction dir);
void on_center();
void on_win();
uint8_t compute_next_index(uint8_t showing, size_t size, int8_t delta);

void main() {
    os_init_scheduler();

    initialize_input();
    os_add_task(check_switches, 100, 1);
    os_add_task(collect_delta, 500, 1);

    format();
    initialize_interactions();
    initialize_display();
    draw_game_map(&map);

    sei();
    while(1);
}

int check_switches(int state) {
    if (get_switch_press(_BV(SWC)) && in_interaction)
        on_center();

    if (get_switch_press(_BV(SWN)))
        on_switch(move_north);

	if (get_switch_press(_BV(SWE)))
        on_switch(move_east);

	if (get_switch_press(_BV(SWS)))
        on_switch(move_south);

	if (get_switch_press(_BV(SWW)))
        on_switch(move_west);

    return state;
}

int collect_delta(int state) {
    int8_t delta = enc_delta();

    if (in_interaction && delta != 0) {
        char on = map.matrix[map.player.z][map.player.y][map.player.x];
        interaction* inter = on == '?'
                             ? find_interaction_by_pos(map.player)
                             : find_interaction_by_char(on);

        if (inter->size_options == 0)
            return state;

        clear_text_box();

        /* Calculate the new index of the question. */
        uint8_t new_index = compute_next_index(inter->showing,
                inter->size_options, delta);

        /* Rewrite the dialogue with a new selected index. */
        write_interaction(inter, inter->current_line, new_index, 1);
    }

    return state;
}

void on_switch(direction dir) {
    /* Move the player in memory. */
    move_to move_res = move_player(&map, dir);
    in_interaction = 0;

    /* Clear the screen. */
    clear_game_map();
    clear_text_box();

    /* Draw in the game map 'box'. */
    draw_game_map(&map);

    /* Draw the dialogue text if any. */
    if (move_res.allowed) {
        interaction* inter = move_res.on == '?'
                             ? find_interaction_by_pos(map.player)
                             : find_interaction_by_char(move_res.on);

        if (inter != NULL) {
            in_interaction = inter->type == npc ? ON_NPC : ON_SCENE;
            uint8_t index = inter->size_options == 0 ? NONE_SELECTED : 0;
            char greet[MAX_LINE_SIZE];
            get_greet_line(greet, inter);
            write_interaction(inter, greet, index, 1);
        }
    } else {
        switch (move_res.on) {
            case '#':
                write_to_text_box("That's a wall, detective.", WHITE);
                break;
            case '=':
                write_to_text_box("A locked door; you might need a key.", WHITE);
                break;
        }
    }
}

void on_center() {
    if (!in_interaction)
        return;

    char on = map.matrix[map.player.z][map.player.y][map.player.x];
    interaction* inter = on == '?'
                         ? find_interaction_by_pos(map.player)
                         : find_interaction_by_char(on);

    uint8_t selected = inter->showing;

    if (on == 'C' && selected == 2)
        on_win();

    uint8_t show_options = 1;
    if (inter->type == scene && inter->on_select_option != NULL)
        show_options = inter->on_select_option(&map, selected);


    clear_game_map();
    clear_text_box();

    draw_game_map(&map);
    char world[MAX_LINE_SIZE];
    get_world_line(world, inter, selected);
    write_interaction(inter, world, selected, show_options);
}

uint8_t compute_next_index(uint8_t showing, size_t size, int8_t delta) {
    uint8_t new_index;
    if (delta > 0) {
        new_index = showing == size - 1 ? 0 : showing + 1;
    } else {
        new_index = showing == 0 ? size - 1 : showing - 1u;
    }

    return new_index;
}

void on_win() {
    clear_text_box();
    write_to_text_box("He's done it again! What a display of wit and tenacity!", YELLOW);
    write_to_text_box("Thank you for playing.", WHITE);

    while(1);

    cli();
}
