#include <stdlib.h>
#include <nes.h>
#include <joystick.h>

#define BALL_COUNT 64

unsigned char x[BALL_COUNT];
unsigned char y[BALL_COUNT];
unsigned char ox[BALL_COUNT];
unsigned char oy[BALL_COUNT];
signed char vx[BALL_COUNT];
signed char vy[BALL_COUNT];
unsigned char colors[BALL_COUNT];

const unsigned char x_raw[] = {202, 148, 156, 114, 165, 100, 194, 45, 168, 73, 194, 135, 153, 136, 108, 121, 8, 110, 146, 145, 146, 169, 173, 148, 194, 167, 78, 194, 117, 137, 14, 131, 25, 133, 73, 127, 119, 156, 144, 94, 97, 105, 42, 10, 166, 166, 178, 100, 143, 59, 27, 132, 123, 6, 63, 6, 108, 169, 137, 118, 126, 101, 126, 17};
const unsigned char y_raw[] = {78, 83, 69, 59, 36, 78, 59, 88, 52, 83, 6, 62, 42, 52, 41, 70, 60, 81, 65, 49, 34, 69, 99, 120, 41, 114, 67, 23, 83, 80, 71, 80, 82, 46, 13, 35, 46, 81, 88, 32, 56, 75, 4, 27, 55, 88, 34, 65, 32, 5, 9, 116, 32, 38, 62, 49, 36, 43, 37, 58, 100, 46, 108, 17};

void init_balls() {
    unsigned char i;
    for(i = 0; i < BALL_COUNT; i++) {
        ox[i] = x[i] = x_raw[i] + 30;
        oy[i] = y[i] = y_raw[i] + 40;
        vx[i] = 0;
        vy[i] = 0;
        if (x_raw[i] < 60)       colors[i] = 1;
        else if (x_raw[i] < 100) colors[i] = 0;
        else if (x_raw[i] < 140) colors[i] = 3;
        else if (x_raw[i] < 180) colors[i] = 1;
        else if (x_raw[i] < 210) colors[i] = 2;
        else                     colors[i] = 0;
    }
}

void update_balls(unsigned char mx, unsigned char my) {
    unsigned char i;
    signed int dx, dy;
    for(i = 0; i < BALL_COUNT; i++) {
        dx = (signed int)mx - x[i];
        dy = (signed int)my - y[i];
        if(abs(dx) + abs(dy) < 25) {
            vx[i] -= (dx >> 2);
            vy[i] -= (dy >> 2);
        }
        vx[i] += ((signed char)ox[i] - (signed char)x[i]) >> 3;
        vy[i] += ((signed char)oy[i] - (signed char)y[i]) >> 3;
        vx[i] = (vx[i] * 4) / 5;
        vy[i] = (vy[i] * 4) / 5;
        x[i] += (vx[i] >> 2);
        y[i] += (vy[i] >> 2);
    }
}

void main(void) {
    unsigned char mx = 128;
    unsigned char my = 120;
    unsigned char pad;
    
    joy_install(joy_static_stddrv);
    init_balls();

    while(1) {
        waitvsync();
        pad = joy_read(JOY_1);
        
        if (pad & JOY_BTN_LEFT)  mx -= 2;
        if (pad & JOY_BTN_RIGHT) mx += 2;
        if (pad & JOY_BTN_UP)    my -= 2;
        if (pad & JOY_BTN_DOWN)  my -= 2;

        update_balls(mx, my);
    }
}
