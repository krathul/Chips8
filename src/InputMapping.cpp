#include "InputMapping.h" 

//keypad button mapping
static uint8_t keypad_buttons[] = {
    SDLK_x, SDLK_1, SDLK_2, SDLK_3,   // 0 1 2 3
    SDLK_q, SDLK_w, SDLK_e, SDLK_a,   // 4 5 6 7
    SDLK_s, SDLK_d, SDLK_z, SDLK_c,   // 8 9 A B
    SDLK_4, SDLK_r, SDLK_f, SDLK_v    // C D E F
};

int InputProcessKeypadEvent(uint32_t key) {
    for (int i = 0;i <= 0xF;i++) {
        if (key == keypad_buttons[i]) {
            return i;
        }
    }
    return -1;
}