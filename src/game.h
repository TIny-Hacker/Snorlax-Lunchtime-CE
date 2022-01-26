#ifndef GAME_H
#define GAME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint8_t draw(uint8_t, int16_t, int16_t, uint8_t, int8_t, int16_t, int16_t);

int16_t eat(int16_t, int16_t, int16_t);

void youLose(int16_t);

#ifdef __cplusplus
}
#endif

#endif
