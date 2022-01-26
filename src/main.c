#include "gfx/gfx.h"
#include "game.h"
#include "rank.h"
#include "globals.h"

#include <tice.h>
#include <graphx.h>
#include <keypadc.h>
#include <stdbool.h>
#include <fileioc.h>

#define TIMER_FREQ 32768
#define ONE_SECOND (TIMER_FREQ / 1)
#define HALF_SECOND (TIMER_FREQ / 2)
#define QRTR_SECOND (TIMER_FREQ / 4)

int16_t highScores[5] = {0, 0, 0, 0, 0};

static void cursor(uint8_t y) {
    gfx_SetDrawBuffer();

    gfx_ZeroScreen();   // Gray border

    gfx_ScaledSprite_NoClip(menu, 16, 24, 3, 3);
    gfx_ScaledTransparentSprite_NoClip(fork, 97, y, 3, 3);
    gfx_ScaledTransparentSprite_NoClip(fork, 205, y, 3, 3);

    gfx_BlitBuffer();
    gfx_SetDrawScreen();
}

static void game(void) {
    uint8_t idleAnimation = 1;
    int16_t score = 0;
    int16_t yum = 0;
    int16_t arrowX = 280;
    uint8_t food = 5;
    uint8_t difficulty = 10;

    rtc_Enable(RTC_SEC_INT);
    srand(rtc_Time());

    timer_Disable(1);

    timer_Set(1, QRTR_SECOND);
    timer_SetReload(1, QRTR_SECOND);

    // Draws the first frame of the game

    draw(0, yum, score, food, 0, 0, 0);

    timer_Enable(1, TIMER_32K, TIMER_0INT, TIMER_DOWN);

    while (!kb_IsDown(kb_KeyClear)) {
        kb_Scan();

        if (score == 174 && highScores[2] > 312 && highScores[2] < 327) {
            food = 4;
        } else {
            food = randInt(0, 3);
        }

        switch (yum) {
            case 12:
                difficulty = yum;
                break;
            case 15:
                difficulty = yum;
                break;
            case 20:
                difficulty = yum;
                break;
            case 30:
                difficulty = yum;
                break;
            default:
                break;
        }

        for (int8_t y = -60; y <= 60; y += difficulty) {              //Falling food loop
            kb_Scan();

            if (kb_IsDown(kb_KeyClear)) {
                break;
            }

            if (timer_ChkInterrupt(1, TIMER_RELOADED)) {
                idleAnimation = draw(idleAnimation, yum, score, food, y, 106, 0);

                timer_AckInterrupt(1, TIMER_RELOADED);
            } else {
                draw(idleAnimation, yum, score, food, y, 106, 0);
            }
        }

        // Arrow moving across the screen

        for (arrowX = 280; arrowX >= 16; arrowX -= 24) {
            kb_Scan();
            
            if (timer_ChkInterrupt(1, TIMER_RELOADED)) {
                idleAnimation = draw(idleAnimation, yum, score, food, 60, 106, arrowX);

                timer_AckInterrupt(1, TIMER_RELOADED);
            } else {
                draw(idleAnimation, yum, score, food, 60, 106, arrowX);
            }

            if (kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyClear)) {
                break;
            }
        }

        if (kb_IsDown(kb_KeyClear)) {
            break;
        }

        // Scoring system

        if (food != 3) {
            if (kb_IsDown(kb_Key2nd)) {
                score = eat(arrowX, score, yum);
                yum++;
            } else {
                youLose(score);
                while (kb_AnyKey());
                return;
            }
        } else {
            if (kb_IsDown(kb_Key2nd)) {
                youLose(score);
                while (kb_AnyKey());
                return;
            }
        }
    }
}

static void rank(void) {
    while (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyAlpha)) {
        // This determines what menu you should be seeing
        
        if (!kb_IsDown(kb_KeyRight)) {
            while (kb_AnyKey());
            rankMenu();
        } else {
            while (kb_AnyKey());
            helpMenu();
        }
    }
}

int main(void) {
    uint8_t cursorY = 124;

    ti_var_t slot = ti_Open("SLXHIGH", "r");    // App variable with the High Scores

    if (slot) {
        ti_Read(&highScores, 10, 1, slot);
    }

    gfx_Begin();

    timer_Disable(1);

    timer_Set(1, HALF_SECOND);
    timer_SetReload(1, HALF_SECOND);

    timer_Enable(1, TIMER_32K, TIMER_0INT, TIMER_DOWN);

    gfx_SetPalette(global_palette, sizeof_global_palette, 0);
    gfx_SetTextFGColor(2);
    gfx_SetTransparentColor(1);
    cursor(cursorY);

    // Main menu stuff, pretty self-explanatory

    while (!kb_IsDown(kb_KeyClear)) {
        kb_Scan();

        switch (kb_Data[7]) {
            case kb_Up:
                if (cursorY >= 144) {
                    cursorY = 124;
                }
                break;

            case kb_Down:
                if (cursorY <= 124) {
                    cursorY = 148;
                }
                break;

            default:
                break;
        }

        // The cursor has an animation as well, so this loop takes care of it

        if (timer_ChkInterrupt(1, TIMER_RELOADED)) {
            if (cursorY == 148 || cursorY == 124) {
                cursorY -= 4;
            } else {
                cursorY += 4;    
            }
            timer_AckInterrupt(1, TIMER_RELOADED);
        }

        // Exiting the menu

        if (kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) {
            if (cursorY >= 144) {
                rank();
            } else {
                game();
            }
        }

        if (!kb_IsDown(kb_KeyClear)) {
            cursor(cursorY);
        }
    }

    gfx_End();

    // Writing the app variable and archiving it

    slot = ti_Open("SLXHIGH", "w+");

    ti_Write(&highScores, 10, 1, slot);

    ti_SetArchiveStatus(true, slot);

    return 0;
}
