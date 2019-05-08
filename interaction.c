#include "interaction.h"
#include <stdlib.h>

interaction interactions[MAX_INTERACTIONS];

uint8_t items_size;


uint8_t on_go_upstairs(game_map* map, uint8_t selected_index);
uint8_t on_go_downstairs(game_map* map, uint8_t selected_index);
uint8_t on_body(game_map* map, uint8_t selected_index);
uint8_t on_box(game_map* map, uint8_t selected_index);

uint16_t store_dialogue(interaction* inter, char* greeting, const char** options);

void initialize_interactions() {
    char greet_buf[MAX_LINE_SIZE];

    /* NPCS */

    /* Guard */
    interaction guard;
    guard.type = npc;
    guard.on_map = 'G';
    strncpy(guard.name, "guard", MAX_NAME_SIZE - 3);
    guard.size_options = 3;
    guard.showing = -1;

    strncpy(greet_buf, "Evening officer!", MAX_LINE_SIZE);

    static const char *guard_dialogue[] = {
        "What's going on?", "The master was found lying dead, officer.",
        "Who are you?", "I've been hired to do guard the property.",
        "Noticed anything suspicious?", "I just heard the cat meow lowdly at some point. It was scary..."
    };

    store_dialogue(&guard, greet_buf, guard_dialogue);

    interactions[0] = guard;

    /* SCENES */

    /* Body */
    interaction body;
    body.type = scene;
    body.pos = (position) {2, 13, 0};
    strncpy(body.name, "body", MAX_NAME_SIZE - 3);
    body.size_options = 0;
    body.showing = -1;

    body.on_select_option = &on_body;

    strncpy(greet_buf, "The master lies dead on the floor in a cold puddle of blood.", MAX_LINE_SIZE);

    store_dialogue(&body, greet_buf, NULL);

    interactions[1] = body;

    /* go_upstairs */
    interaction go_upstairs;
    go_upstairs.type = scene;
    go_upstairs.pos = (position) {6, 16, 0};
    strncpy(go_upstairs.name, "up", MAX_NAME_SIZE - 3);
    go_upstairs.size_options = 1;
    go_upstairs.showing = -1;

    go_upstairs.on_select_option = &on_go_upstairs;


    strncpy(greet_buf, "A wodden staircase.", MAX_LINE_SIZE);

    static const char *go_upstairs_text[] = {
        "Go upstairs.", "You climb the shoddy stairs."
    };

    store_dialogue(&go_upstairs, greet_buf, go_upstairs_text);

    interactions[2] = go_upstairs;

    /* go_downstairs */
    interaction go_downstairs;
    go_downstairs.type = scene;
    go_downstairs.pos = (position) {6, 16, 1};
    strncpy(go_downstairs.name, "down", MAX_NAME_SIZE - 3);
    go_downstairs.size_options = 1;
    go_downstairs.showing = -1;

    go_downstairs.on_select_option = &on_go_downstairs;

    strncpy(greet_buf, "A wodden staircase.", MAX_LINE_SIZE);

    static const char *go_downstairs_text[] = {
        "Go downstairs.", "The wood squeaks under your weight. You are now downstairs."
    };

    store_dialogue(&go_downstairs, greet_buf, go_downstairs_text);

    interactions[3] = go_downstairs;

    /* box */
    interaction box;
    box.type = scene;
    box.pos = (position) {1, 1, 1};
    strncpy(box.name, "box", MAX_NAME_SIZE - 3);
    box.size_options = 1;
    box.showing = -1;

    box.on_select_option = &on_box;

    strncpy(greet_buf, "The cat's litter box.", MAX_LINE_SIZE);

    static const char *box_text[] = {
        "Inspect.", "You find a bloddy knife covered by the litter and large amounts of catnip."
    };

    store_dialogue(&box, greet_buf, box_text);

    interactions[4] = box;

    /* Cat */
    interaction cat;
    cat.type = npc;
    cat.on_map = 'C';
    strncpy(cat.name, "cat", MAX_NAME_SIZE - 3);
    cat.size_options = 1;
    cat.showing = -1;

    cat.on_select_option = NULL;

    strncpy(greet_buf, "An innocent looking cat. \"Meow!\"", MAX_LINE_SIZE);

    static const char *cat_dialogue[] = {
        "Pet the cat.", "Meow, Meow."
    };

    store_dialogue(&cat, greet_buf, cat_dialogue);

    interactions[5] = cat;
}

interaction* find_interaction_by_char(const char c) {
    for (size_t i = 0; i < MAX_INTERACTIONS; ++i)
        if (interactions[i].on_map == c)
            return &interactions[i];

    return NULL;
}

interaction* find_interaction_by_pos(position pos) {
    for (size_t i = 0; i < MAX_INTERACTIONS; ++i)
        if (pos.x == interactions[i].pos.x
            && pos.y == interactions[i].pos.y
            && pos.z == interactions[i].pos.z)
            return &interactions[i];

    return NULL;
}

interaction* find_interaction_by_name(const char* name) {
    for (size_t i = 0; i < MAX_INTERACTIONS; ++i)
        if (strcmp(interactions[i].name, name) == 0)
            return &interactions[i];

    return NULL;
}

void get_player_line(char* buf, interaction* inter, size_t index) {
    if (index > inter->size_options)
        return;

    char name_buf[MAX_NAME_SIZE];

    snprintf(name_buf, MAX_NAME_SIZE, "%u%s.p", index, inter->name);
    result r = getFile(name_buf, buf, MAX_LINE_SIZE);

    if (r != NO_ERROR) {
        char error_buf[10] = "ERROR: ";
        itoa((int) r, error_buf + 7, 10);
        strncpy(buf, error_buf, MAX_LINE_SIZE);
    }
}

void get_world_line(char* buf, interaction* inter, size_t index) {
    if (index > inter->size_options)
        return;

    char name_buf[MAX_NAME_SIZE];

    snprintf(name_buf, MAX_NAME_SIZE, "%u%s.w", index, inter->name);
    result r = getFile(name_buf, buf, MAX_LINE_SIZE);

    if (r != NO_ERROR) {
        char error_buf[10] = "ERROR: ";
        itoa((int) r, error_buf + 7, 10);
        strncpy(buf, error_buf, MAX_LINE_SIZE);
    }
}

void get_greet_line(char* buf, interaction* inter) {
    char name_buf[MAX_NAME_SIZE];

    snprintf(name_buf, MAX_NAME_SIZE, "%s.g", inter->name);
    result r = getFile(name_buf, buf, MAX_LINE_SIZE);

    if (r != NO_ERROR) {
        char error_buf[10] = "ERROR: ";
        itoa((int) r, error_buf + 7, 10);
        strncpy(buf, error_buf, MAX_LINE_SIZE);
    }
}

uint16_t add_line(interaction* inter, const char* player_line, const char* world_line) {
    if (inter->size_options == MAX_OPTIONS)
        return 0;

    inter->size_options++;

    uint16_t error = 0;
    char name_buf[MAX_NAME_SIZE];

    snprintf(name_buf, MAX_NAME_SIZE, "%u%s.p", inter->size_options - 1, inter->name);
    strncpy(inter->options[inter->size_options - 1].player_file, name_buf, MAX_NAME_SIZE);
    error ^= (uint16_t) newFile(name_buf, player_line, MAX_LINE_SIZE, 0);

    snprintf(name_buf, MAX_NAME_SIZE, "%u%s.w", inter->size_options - 1, inter->name);
    strncpy(inter->options[inter->size_options - 1].world_file, name_buf, MAX_NAME_SIZE);
    error ^= (uint16_t) newFile(name_buf, world_line, MAX_LINE_SIZE, 0);
    
    return error;
}

uint16_t remove_line(interaction* inter, const uint8_t index) {
    if (index >= inter->size_options)
        return 0;

    uint16_t error = 0;
    inter->size_options--;

    error ^= (uint16_t) deleteFile(inter->options[index].player_file);
    error ^= (uint16_t) deleteFile(inter->options[index].world_file);

    return error;
}

uint16_t store_dialogue(interaction* inter, char* greeting, const char** options) {
    uint16_t error = 0;
    char name_buf[MAX_NAME_SIZE];

    snprintf(name_buf, MAX_NAME_SIZE, "%s.g", inter->name);
    strncpy(inter->greet_file, name_buf, MAX_NAME_SIZE);
    error ^= (uint16_t) newFile(name_buf, greeting, MAX_LINE_SIZE, 0);

    for (size_t i = 0; i < inter->size_options * 2; i += 2) {
        snprintf(name_buf, MAX_NAME_SIZE, "%u%s.p", i / 2, inter->name);
        strncpy(inter->options[i / 2].player_file, name_buf, MAX_NAME_SIZE);
        error ^= (uint16_t) newFile(name_buf, options[i], MAX_LINE_SIZE, 0);

        snprintf(name_buf, MAX_NAME_SIZE, "%u%s.w", i / 2, inter->name);
        strncpy(inter->options[i / 2].world_file, name_buf, MAX_NAME_SIZE);
        error ^= (uint16_t) newFile(name_buf, options[i + 1], MAX_LINE_SIZE, 0);
    }

    return error;
}

uint8_t went_upstairs = 0;
uint8_t inspected_litter = 0;

uint8_t on_go_upstairs(game_map* map, uint8_t selected_index) {
    if (selected_index != 0)
        return 0;
    
    /* Move the player. */
    move_player(map, move_up);

    if (went_upstairs)
        return 0;

    went_upstairs = 1;

    /* Update the interactions and add the 'key' to the inventory. */
    interaction* guard = find_interaction_by_name("guard");
    if (guard != NULL) {
        add_line(guard, "Where does the door upstairs lead to?",
                "The master's bathroom, but I don't have the key. Be careful though! The cat is in there.");
    }

    interaction* body = find_interaction_by_name("body");
    if (body != NULL) {
        add_line(body, "Search the body.", "You find a key in one of the pockets.");
    }

    return 0;
}

uint8_t on_go_downstairs(game_map* map, uint8_t selected_index) {
    if (selected_index != 0)
        return 0;

    move_player(map, move_down);
    return 0;
}

uint8_t on_body(game_map* map, uint8_t selected_index) {
    if (selected_index != 0)
        return 0;

    if (has_item("key")) {
        interaction* body = find_interaction_by_name("body");
        deleteFile(body->options[0].world_file);
        newFile(body->options[0].world_file, "You find nothing.", MAX_LINE_SIZE, 0);
    }

    add_item("key");

    return 1;
}

uint8_t on_box(game_map* map, uint8_t selected_index) {
    if (selected_index != 0)
        return 0;

    if (inspected_litter)
        return 1;

    inspected_litter = 1;
    interaction* cat = find_interaction_by_name("cat");
    if (cat != NULL) {
        add_line(cat, "Are you high?", "Meowbe.");
        add_line(cat, "You are under arrest for capital murder!", "Meow...");
    }

    return 1;
}
