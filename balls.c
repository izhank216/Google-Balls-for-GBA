#include <tonc.h>
#include <stdlib.h>

#define FP_SHIFT 8
#define TO_FP(n) ((n) << FP_SHIFT)
#define FROM_FP(n) ((n) >> FP_SHIFT)

typedef struct {
    int x, y, ox, oy, vx, vy;
    u16 color;
} Ball;

#define BALL_COUNT 65
Ball balls[BALL_COUNT];

void init_balls() {
    // These are scaled-down coordinates to center the logo on a 240x160 screen
    int x_raw[] = {202, 348, 256, 214, 265, 300, 294, 45, 268, 73}; // truncated for brevity
    int y_raw[] = {78, 83, 69, 59, 36, 78, 59, 88, 52, 83};
    u16 colors[] = {CLR_ORANGE, CLR_RED, CLR_BLUE, CLR_YELLOW, CLR_GREEN, CLR_BLUE};

    for(int i = 0; i < BALL_COUNT; i++) {
        // We scale the original 400ish range coordinates by ~0.5 to fit 240
        int target_x = (x_raw[i % 10] / 2) + 40; 
        int target_y = (y_raw[i % 10] / 2) + 40;

        balls[i].ox = balls[i].x = TO_FP(target_x);
        balls[i].oy = balls[i].y = TO_FP(target_y);
        balls[i].vx = balls[i].vy = 0;
        balls[i].color = colors[i % 6];
    }
}

void update_balls(int mx, int my) {
    int f_mx = TO_FP(mx);
    int f_my = TO_FP(my);

    for(int i = 0; i < BALL_COUNT; i++) {
        int dx = f_mx - balls[i].x;
        int dy = f_my - balls[i].y;
        
        // Use FROM_FP to prevent integer overflow during squaring
        int dist_sq = (FROM_FP(dx)*FROM_FP(dx)) + (FROM_FP(dy)*FROM_FP(dy));

        // Interaction radius (approx 40 pixels)
        if(dist_sq < 1600) {
            balls[i].vx -= dx / 16;
            balls[i].vy -= dy / 16;
        }

        // Spring physics (return to home)
        balls[i].vx += (balls[i].ox - balls[i].x) / 32;
        balls[i].vy += (balls[i].oy - balls[i].y) / 32;

        // Friction
        balls[i].vx = (balls[i].vx * 9) / 10;
        balls[i].vy = (balls[i].vy * 9) / 10;

        balls[i].x += balls[i].vx;
        balls[i].y += balls[i].vy;
    }
}

int main() {
    REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;
    
    irq_init(NULL);
    irq_add(II_VBLANK, NULL);

    init_balls();

    int mx = 120;
    int my = 80;

    while(1) {
        VBlankIntrWait();
        key_poll();

        // D-Pad movement for the "Mouse"
        mx += 3 * key_tri_horz();
        my += 3 * key_tri_vert();

        mx = clamp(mx, 0, 239);
        my = clamp(my, 0, 159);

        // Standard Mode 3 clear
        m3_fill(CLR_WHITE);

        for(int i = 0; i < BALL_COUNT; i++) {
            int px = FROM_FP(balls[i].x);
            int py = FROM_FP(balls[i].y);
            
            // Draw 2x2 pixels if within bounds
            if(px > 0 && px < 239 && py > 0 && py < 159) {
                m3_plot(px, py, balls[i].color);
                m3_plot(px+1, py, balls[i].color);
                m3_plot(px, py+1, balls[i].color);
                m3_plot(px+1, py+1, balls[i].color);
            }
        }
        
        update_balls(mx, my);
        
        // Draw the black cursor square
        m3_rect(mx-1, my-1, mx+1, my+1, CLR_BLACK);
    }
    return 0;
}
