#include <tice.h>
#include <graphx.h>
#include <stdint.h>
#include <keypadc.h>
#include <stdbool.h>

#include "gfx/gfx.h"
#include "rank.h"
#include "globals.h"

void rankMenu(void) {
    gfx_SetDrawBuffer();

    gfx_ZeroScreen();

    gfx_ScaledSprite_NoClip(ranking, 16, 24, 3, 3);
    
    gfx_SetTextScale(3, 3);
    gfx_SetTextXY(56, 70);
    gfx_PrintInt(highScores[0], 1);

    gfx_SetTextXY(56, 93);
    gfx_PrintInt(highScores[1], 1);

    gfx_SetTextXY(56, 116);
    gfx_PrintInt(highScores[2], 1);

    gfx_SetTextXY(56, 139);
    gfx_PrintInt(highScores[3], 1);

    gfx_SetTextXY(56, 162);
    gfx_PrintInt(highScores[4], 1);
    

    gfx_BlitBuffer();
    gfx_SetDrawScreen();

    while (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyAlpha) && !kb_IsDown(kb_KeyRight)) {
        kb_Scan();
    }
}

void helpMenu(void) {
    gfx_SetDrawBuffer();

    gfx_ZeroScreen();
    
    gfx_ScaledSprite_NoClip(help, 16, 24, 3, 3);

    gfx_SetTextScale(2, 2);

    gfx_PrintStringXY("Use the arrow", 95, 75);
    gfx_PrintStringXY("keys to move", 95, 90);
    gfx_PrintStringXY("through menus.", 95, 105);

    gfx_PrintStringXY("Use 2nd to eat.", 95, 132);

    gfx_PrintStringXY("Use alpha to", 95, 153);
    gfx_PrintStringXY("return.", 95, 167);

    gfx_BlitBuffer();
    gfx_SetDrawScreen();

    while (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyAlpha) && !kb_IsDown(kb_KeyLeft)) {
        kb_Scan();
    }
}
