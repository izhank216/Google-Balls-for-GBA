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
    // Full coordinate data from your original desktop code
    const int x_raw[] = {202, 348, 256, 214, 265, 300, 294, 45, 268, 73, 294, 235, 353, 336, 208, 321, 8, 180, 146, 145, 246, 169, 273, 248, 294, 267, 78, 294, 117, 137, 14, 331, 25, 233, 73, 327, 319, 256, 244, 194, 97, 105, 42, 10, 166, 266, 178, 100, 343, 59, 27, 232, 123, 6, 63, 6, 108, 169, 137, 318, 226, 101, 226, 17, 232};
    const int y_raw[] = {78, 83, 69, 59, 36, 78, 59, 88, 52, 83, 6, 62, 42, 52, 41, 70, 60, 81, 65, 49, 34, 69, 99, 120, 41, 114, 67, 23, 83, 80, 71, 80, 82, 46, 13, 35, 46, 81, 88, 32, 56, 75, 4, 27, 55, 88, 34, 65, 32, 5, 9, 116, 32, 38, 62, 49, 36, 43, 37, 58, 100, 46, 108, 17, 93};
    const u16 colors[] = {CLR_ORANGE, CLR_RED, CLR_BLUE, CLR_YELLOW, CLR_BLUE, CLR_GREEN};

    for(int i = 0; i < BALL_COUNT; i++) {
        // Scaling: original was ~360 wide, GBA is 240. (x * 0.5) + offset centers it.
        int target_x = (x_raw[i] / 2) + 35; 
        int target_y = (y_raw[i] / 2) + 45;

        balls[i].ox = balls[i].x = TO_FP(target_x);
        balls[i].oy = balls[i].y = TO_FP(target_y);
        balls[i].vx = balls[i].vy = 0;
        
        // Match the color logic
        if (x_raw[i] < 90) balls[i].color = CLR_BLUE;      // 'G'
        else if (x_raw[i] < 150) balls[i].color = CLR_RED;  // 'o'
        else if (x_raw[i] < 210) balls[i].color = CLR_ORANGE; // 'o'
        else if (x_raw[i] < 270) balls[i].color = CLR_BLUE;  // 'g'
        else if (x_raw[i] < 310) balls[i].color = CLR_GREEN; // 'l'
        else balls[i].color = CLR_RED;                       // 'e'
    }
}

void update_balls(int mx, int my) {
    int f_mx = TO_FP(mx);
    int f_my = TO_FP(my);

    for(int i = 0; i < BALL_COUNT; i++) {
        int dx = f_mx - balls[i].x;
        int dy = f_my - balls[i].y;
        
        // Approximate distance check (Manhattan distance for GBA speed)
        int abs_dx = abs(FROM_FP(dx));
        int abs_dy = abs(FROM_FP(dy));

        // If "mouse" is within ~35 pixels
        if(abs_dx + abs_dy < 35) {
            balls[i].vx -= dx / 12;
            balls[i].vy -= dy / 12;
        }

        // Return to home physics
        balls[i].vx += (balls[i].ox - balls[i].x) / 24;
        balls[i].vy += (balls[i].oy - balls[i].y) / 24;

        // Friction
        balls[i].vx = (balls[i].vx * 200) / 256;
        balls[i].vy = (balls[i].vy * 200) / 256;

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

        // Keyboard/D-Pad Controls
        mx += 3 * key_tri_horz();
        my += 3 * key_tri_vert();

        mx = clamp(mx, 5, 234);
        my = clamp(my, 5, 154);

        m3_fill(CLR_WHITE);

        for(int i = 0; i < BALL_COUNT; i++) {
            int px = FROM_FP(balls[i].x);
            int py = FROM_FP(balls[i].y);
            
            // Draw 2x2 blocks for balls
            m3_rect(px, py, px+2, py+2, balls[i].color);
        }
        
        update_balls(mx, my);
        
        // Cursor
        m3_rect(mx-1, my-1, mx+1, my+1, CLR_BLACK);
    }
    return 0;
}
