#include "game.h"
#include "gfx/gfx.h"
#include "globals.h"

#include <tice.h>
#include <graphx.h>
#include <keypadc.h>
#include <stdbool.h>

#define TIMER_FREQ 32768
#define ONE_SECOND (TIMER_FREQ / 1)
#define HALF_SECOND (TIMER_FREQ / 2)
#define QRTR_SECOND (TIMER_FREQ / 4)

static void newBest(int score) {
    gfx_ScaledSprite_NoClip(newHighScore, 16, 24, 3, 3);
    gfx_SetTextXY(96, 118);
    gfx_SetTextScale(4, 4);

    gfx_PrintInt(score, 4);

    while (!kb_IsDown(kb_Key2nd) && !kb_IsDown(kb_KeyClear)) {
        kb_Scan();
    }
}

uint8_t draw(uint8_t idleAnimation, int yum, int score, uint8_t food, int8_t y, int x, int arrowX) {
    gfx_SetDrawBuffer();

    gfx_ZeroScreen();   // Gray border
    gfx_SetColor(3);
    gfx_FillRectangle_NoClip(16, 24, 288, 192); // Background

    // The animation of the snorlax

    if (!idleAnimation) {
        gfx_ScaledSprite_NoClip(idle1, 52, 45, 3, 3);
        idleAnimation = 1;
    } else if (idleAnimation == 1) {
        gfx_ScaledSprite_NoClip(idle2, 58, 45, 3, 3);
        idleAnimation = 2;
    } else {
        gfx_ScaledSprite_NoClip(idle3, 61, 45, 3, 3);
        idleAnimation = 0;
    }

    // HUD stuff

    gfx_ScaledSprite_NoClip(topLeft, 16, 24, 3, 3);
    gfx_ScaledSprite_NoClip(topRight, 226, 24, 3, 3);
    gfx_ScaledSprite_NoClip(bottom, 16, 165, 3, 3);

    // Detects whether or not the arrow should be displayed and if so it displays it

    if (arrowX) {
        gfx_ScaledTransparentSprite_NoClip(arrow, arrowX, 189, 3, 3);
    }

    // Draws whatever food should be drawn, otherwise draws nothing

    switch (food) {
        case 0:
            gfx_TransparentSprite(bread, x, y + 48);
            break;
        case 1:
            gfx_TransparentSprite(pineapple, x + 18, y + 9);
            break;
        case 2:
            gfx_TransparentSprite(melon, x + 9, y + 15);
            break;
        case 3:
            gfx_TransparentSprite(pichu1, x + 21, y);
            break;
        case 4:
            gfx_TransparentSprite(secret, x + 28, y + 27);
            break;
        default:
            break;
    }

    // Covers up the top of the sprite if it goes on the border

    gfx_SetColor(0);
    gfx_FillRectangle_NoClip(95, 0, 148, 24);

    // Draws the scores

    gfx_SetTextScale(2, 2);

    gfx_SetTextXY(25, 34);
    gfx_PrintInt(score, 3);

    gfx_SetTextXY(243, 34);
    gfx_PrintInt(yum, 3);

    gfx_BlitBuffer();
    gfx_SetDrawScreen();

    return idleAnimation;   // In some cases it is necessary to change the frame of the animation
}

int eat(int speed, int score, int yum) {
    gfx_SetDrawBuffer();

    // Partially erases background and draws Snorlax

    gfx_SetColor(3);
    gfx_FillRectangle_NoClip(52, 45, 216, 120);

    // How fast you pressed "2nd"

    if (speed < 87) {
        gfx_ScaledSprite_NoClip(ok, 16, 168, 3, 3);
        score++;
    } else if (speed > 207) {
        gfx_ScaledSprite_NoClip(great, 208, 168, 3, 3);
        score += 10;
    } else {
        gfx_ScaledSprite_NoClip(good, 88, 168, 3, 3);
        score += 5;
    }

    // Snorlax eating

    gfx_ScaledSprite_NoClip(eatFood, 85, 74, 3, 3);

    // Displaying the score

    gfx_ScaledSprite_NoClip(topLeft, 16, 24, 3, 3);
    gfx_ScaledSprite_NoClip(topRight, 226, 24, 3, 3);

    gfx_SetTextXY(25, 34);
    gfx_PrintInt(score, 3);

    gfx_SetTextXY(243, 34);
    gfx_PrintInt(yum + 1, 3);

    gfx_BlitBuffer();
    gfx_SetDrawScreen();

    msleep(299);    // There is a short pause for the eating frame

    return score;
}

void youLose(int score) {
    gfx_SetDrawBuffer();

    gfx_ZeroScreen();   // Gray border that is everywhere

    gfx_ScaledSprite_NoClip(gameOver, 16, 24, 3, 3);

    gfx_BlitBuffer();
    gfx_SetDrawScreen();

    // I had to make sure the timer was all resolved because of some bugs

    if (timer_ChkInterrupt(1, TIMER_RELOADED)) {
        timer_AckInterrupt(1, TIMER_RELOADED);
    }

    timer_Disable(1);

    // Modifies the timer

    timer_Set(1, ONE_SECOND);
    timer_SetReload(1, ONE_SECOND);

    timer_Enable(1, TIMER_32K, TIMER_0INT, TIMER_DOWN);

    // Waits on the Game Over screen unless you press a button or a second has passed
    
    while (!timer_ChkInterrupt(1, TIMER_RELOADED) && !kb_IsDown(kb_KeyClear)) {
        kb_Scan();
    }

    while (kb_AnyKey());

    // Highscore code provided by RoccoLox Programs

    int8_t offset;

    // Check to see if there's a new highscore
    for (offset = 0; offset < 4; offset++) {
        if (highScores[offset] < score) break;
    }

    if (offset < 4) {   // If there's a new highscore
        for (uint8_t listOffset = 4; listOffset > offset; listOffset--) {
            highScores[listOffset] = highScores[listOffset - 1];    // Move all of the lower highscores down
        }

        highScores[offset] = score;
        newBest(score);
    } else if (highScores[4] < score) {
        highScores[4] = score;
        newBest(score);
    }
}
