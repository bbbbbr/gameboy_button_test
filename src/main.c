#include <gbdk/platform.h>
#include <stdio.h>
#include <gbdk/console.h>

#include "input.h"
#include "../res/layout_map.h"
#include "../res/layout_tiles.h"


// Indexes for button counters
enum {
    C_UP = 0,
    C_DOWN,
    C_LEFT,
    C_RIGHT,

    C_SELECT,
    C_START,
    C_A,
    C_B,

    C_UL,
    C_UR,
    C_DL,
    C_DR,

    C_MAX
};

// Should match C_* enum order
// x,y
const uint8_t print_at[][2] = {
    {  8, 1  },// Up
    {  8,13  },// Down
    {  1, 7  },// Left
    { 15, 7  },// Right

    { 16, 13 },// Select
    { 16, 14 },// Start
    { 16, 15 },// A
    { 16, 16 },// B

    {  3,  4 }, // U-L
    { 13,  4 }, // U-R
    {  3, 10 }, // D-L
    { 13, 10 }  // D-R
};

enum {
    MODE_4_WAY,
    MODE_8_WAY
};

uint16_t button_counters[C_MAX];
uint8_t disp_mode = MODE_4_WAY;



void init_gfx() {
    printf("x"); // Shortcut to initialize stdio stuff before gfx load so it doesn't get overwritten

    // Load Background tiles and then map (start 1t 128 instead of 0
    set_bkg_data(128, layout_tiles_count, layout_tiles);
    set_bkg_tiles(0, 0, layout_map_width, layout_map_height, layout_map);

    // Turn the background map on to make it visible
    SHOW_BKG;
}


// Reset all button counters and display them (or hide if needed)
void reset_counters(void) {
    uint8_t c;
    for (c = 0; c < C_MAX; c++) {
        button_counters[c] = 0;
        gotoxy(print_at[c][0], print_at[c][1]);

        // Don't print 8-way outputs if in 4-way mode
        if ((c < 8) || (disp_mode == MODE_8_WAY)) {
            printf("%x", button_counters[c]);
        }
        else printf("    ");
    }
}


// Increment a button counter and display it
inline void increment_counter(uint8_t counter_id) {
    button_counters[counter_id]++;
    gotoxy(print_at[counter_id][0], print_at[counter_id][1]);
    printf("%x", button_counters[ counter_id ]);
}


// Update and display all buttons
// Perform resets and mode changes if requested
void update_buttons() {

    // Read hardware buttons
    UPDATE_KEYS();

    // D-Pad
    if (disp_mode == MODE_4_WAY) {
        // 4-Way Mode
        if (KEY_PRESSED(J_UP))    increment_counter(C_UP);
        if (KEY_PRESSED(J_DOWN))  increment_counter(C_DOWN);
        if (KEY_PRESSED(J_LEFT))  increment_counter(C_LEFT);
        if (KEY_PRESSED(J_RIGHT)) increment_counter(C_RIGHT);
    }
    else {
        // 8-Way mode, try diagonals first
        if      (KEY_PRESSED(J_UP) && KEY_PRESSED(J_LEFT)) increment_counter(C_UL);
        else if (KEY_PRESSED(J_UP) && KEY_PRESSED(J_RIGHT)) increment_counter(C_UR);
        else if (KEY_PRESSED(J_DOWN) && KEY_PRESSED(J_LEFT)) increment_counter(C_DL);
        else if (KEY_PRESSED(J_DOWN) && KEY_PRESSED(J_RIGHT)) increment_counter(C_DR);
        else {
            // Then test others if applicable
            if (KEY_PRESSED(J_UP))    increment_counter(C_UP);
            if (KEY_PRESSED(J_DOWN))  increment_counter(C_DOWN);
            if (KEY_PRESSED(J_LEFT))  increment_counter(C_LEFT);
            if (KEY_PRESSED(J_RIGHT)) increment_counter(C_RIGHT);
        }
    }


    // Buttons
    if (KEY_PRESSED(J_SELECT))  increment_counter(C_SELECT);
    if (KEY_PRESSED(J_START)) increment_counter(C_START);
    if (KEY_PRESSED(J_A))    increment_counter(C_A);
    if (KEY_PRESSED(J_B))  increment_counter(C_B);

    if (KEY_PRESSED(J_SELECT) && KEY_PRESSED(J_B)) {
        reset_counters();
        wait_no_keys_lowcpu();
    }
    else if (KEY_PRESSED(J_SELECT) && KEY_PRESSED(J_A)) {
        // Change mode and reset screen
        if (disp_mode == MODE_4_WAY) disp_mode = MODE_8_WAY;
        else disp_mode = MODE_4_WAY;
        reset_counters();
        wait_no_keys_lowcpu();
    }
}



void main(void)
{
    init_gfx();

    reset_counters();

    // Loop forever
    while(1) {

        update_buttons();

        // Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
}