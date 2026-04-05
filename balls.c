#include <gba.h>
#include <tonc.h>
#include <stdlib.h>

#define FP_SHIFT 8
#define TO_FP(n) ((n) << FP_SHIFT)
#define FROM_FP(n) ((n) >> FP_SHIFT)

typedef struct {
    int x, y, ox, oy, vx, vy;
    u16 color;
} Ball;

#define BALL_COUNT 64
Ball balls[BALL_COUNT];

void init_balls() {
    int x_coords[] = {101, 174, 128, 107, 132, 150}; 
    int y_coords[] = {39, 41, 34, 29, 18, 39};
    u16 colors[] = {CLR_ORANGE, CLR_RED, CLR_BLUE, CLR_YELLOW, CLR_BLUE, CLR_GREEN};

    for(int i = 0; i < BALL_COUNT; i++) {
        int idx = i % 6;
        balls[i].ox = balls[i].x = TO_FP(x_coords[idx] + (i % 30));
        balls[i].oy = balls[i].y = TO_FP(y_coords[idx] + (i / 10));
        balls[i].vx = balls[i].vy = 0;
        balls[i].color = colors[idx];
    }
}

void update_balls(int mx, int my) {
    int f_mx = TO_FP(mx);
    int f_my = TO_FP(my);

    for(int i = 0; i < BALL_COUNT; i++) {
        int dx = f_mx - balls[i].x;
        int dy = f_my - balls[i].y;
        int dist_sq = (FROM_FP(dx)*FROM_FP(dx)) + (FROM_FP(dy)*FROM_FP(dy));

        if(dist_sq < 1600) {
            balls[i].vx -= dx / 4;
            balls[i].vy -= dy / 4;
        }

        balls[i].vx += (balls[i].ox - balls[i].x) / 16;
        balls[i].vy += (balls[i].oy - balls[i].y) / 16;
        balls[i].vx = (balls[i].vx * 7) / 8;
        balls[i].vy = (balls[i].vy * 7) / 8;
        balls[i].x += balls[i].vx;
        balls[i].y += balls[i].vy;
    }
}

int main() {
    REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;
    
    irq_init(NULL);
    irq_enable(II_VBLANK);

    init_balls();

    int mx = 120;
    int my = 80;

    while(1) {
        VBlankIntrWait();
        
        u16 mouse_data = REG_SIODATA32;
        int dx = (int8_t)(mouse_data & 0xFF);
        int dy = (int8_t)((mouse_data >> 8) & 0xFF);
        
        mx += dx;
        my += dy;

        if(mx < 0) mx = 0; if(mx > 239) mx = 239;
        if(my < 0) my = 0; if(my > 159) my = 159;

        m3_fill(CLR_WHITE);
        for(int i = 0; i < BALL_COUNT; i++) {
            m3_plot(FROM_FP(balls[i].x), FROM_FP(balls[i].y), balls[i].color);
            m3_plot(FROM_FP(balls[i].x)+1, FROM_FP(balls[i].y), balls[i].color);
            m3_plot(FROM_FP(balls[i].x), FROM_FP(balls[i].y)+1, balls[i].color);
        }
        
        update_balls(mx, my);
        m3_plot(mx, my, CLR_BLACK);
    }
    return 0;
}
