#include <tice.h>
#include <graphx.h>
#include <stdint.h>
#include <keypadc.h>
#include <stdbool.h>
#include <fileioc.h>

#include "gfx/gfx.h"
#include "game.h"
#include "rank.h"
#include "globals.h"

#define TIMER_FREQ 32768
#define ONE_SECOND (TIMER_FREQ / 1)
#define HALF_SECOND (TIMER_FREQ / 2)
#define QRTR_SECOND (TIMER_FREQ / 4)

int16_t highScores[5] = {0, 0, 0, 0, 0};

static void cursor(uint8_t y) {
    gfx_SetDrawBuffer();

    gfx_ZeroScreen();

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

    timer_Disable(1);

    timer_Set(1, QRTR_SECOND);
    timer_SetReload(1, QRTR_SECOND);

    draw(0, yum, score, 5, 0, 0, 0);

    timer_Enable(1, TIMER_32K, TIMER_0INT, TIMER_DOWN);

    while (!kb_IsDown(kb_KeyClear)) {
        kb_Scan();

        uint8_t food = randInt(0, 3);

        for (int8_t y = -80; y <= 40; y += 10) {              //Falling food loop
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

        for (arrowX = 280; arrowX >= 16; arrowX -= 24) {
            kb_Scan();
            
            if (timer_ChkInterrupt(1, TIMER_RELOADED)) {
                idleAnimation = draw(idleAnimation, yum, score, food, 40, 106, arrowX);

                timer_AckInterrupt(1, TIMER_RELOADED);
            } else {
                draw(idleAnimation, yum, score, food, 40, 106, arrowX);
            }

            if (kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyClear)) {
                break;
            }
        }

        if (kb_IsDown(kb_KeyClear)) {
            break;
        }

        if (food < 3) {
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

    ti_var_t slot = ti_Open("SLXHIGH", "r");

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

        if (timer_ChkInterrupt(1, TIMER_RELOADED)) {
            if (cursorY == 148 || cursorY == 124) {
                cursorY -= 4;
            } else {
                cursorY += 4;    
            }
            timer_AckInterrupt(1, TIMER_RELOADED);
        }

        if (kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) {
            if (cursorY >= 144) {
                rank();
            } else {
                game();
            }
        }

        cursor(cursorY);
    }

    gfx_End();

    slot = ti_Open("SLXHIGH", "w+");

    ti_Write(&highScores, 10, 1, slot);

    ti_SetArchiveStatus(true, slot);

    return 0;
}