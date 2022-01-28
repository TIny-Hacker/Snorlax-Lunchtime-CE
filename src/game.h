#ifndef GAME_H
#define GAME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint8_t draw(uint8_t, int,  int,  uint8_t, int8_t, int,  int);

int eat(int,  int,  int);

void youLose(int);

#ifdef __cplusplus
}
#endif

#endif
