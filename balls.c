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
        // Spread balls out slightly around the logo coordinates
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
        
        // Use fixed point for distance check
        int d_x = FROM_FP(dx);
        int d_y = FROM_FP(dy);
        int dist_sq = (d_x * d_x) + (d_y * d_y);

        // Interaction: Push away
        if(dist_sq < 1600) {
            balls[i].vx -= dx / 8;
            balls[i].vy -= dy / 8;
        }

        // Return to home
        balls[i].vx += (balls[i].ox - balls[i].x) / 16;
        balls[i].vy += (balls[i].oy - balls[i].y) / 16;
        
        // Friction
        balls[i].vx = (balls[i].vx * 7) / 8;
        balls[i].vy = (balls[i].vy * 7) / 8;
        
        balls[i].x += balls[i].vx;
        balls[i].y += balls[i].vy;
    }
}

int main() {
    // Mode 3 (Bitmap), Background 2 enabled
    REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;
    
    irq_init(NULL);
    irq_add(II_VBLANK, NULL);

    init_balls();

    int mx = 120;
    int my = 80;

    while(1) {
        VBlankIntrWait();
        key_poll();

        // Mouse/Cursor control via D-Pad
        mx += 2 * key_tri_horz();
        my += 2 * key_tri_vert();

        // Keep cursor on screen
        mx = clamp(mx, 0, 239);
        my = clamp(my, 0, 159);

        // Clear screen with White
        m3_fill(CLR_WHITE);

        for(int i = 0; i < BALL_COUNT; i++) {
            int draw_x = FROM_FP(balls[i].x);
            int draw_y = FROM_FP(balls[i].y);
            
            // Draw 2x2 ball for visibility
            m3_plot(draw_x, draw_y, balls[i].color);
            m3_plot(draw_x + 1, draw_y, balls[i].color);
            m3_plot(draw_x, draw_y + 1, balls[i].color);
            m3_plot(draw_x + 1, draw_y + 1, balls[i].color);
        }
        
        update_balls(mx, my);
        
        // Draw cursor
        m3_plot(mx, my, CLR_BLACK);
        m3_plot(mx+1, my, CLR_BLACK);
        m3_plot(mx, my+1, CLR_BLACK);
    }
    
    return 0;
}
