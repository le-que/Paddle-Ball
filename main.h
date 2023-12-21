#ifndef MAIN_H
#define MAIN_H

#include "gba.h"

// TODO: Create any necessary structs
typedef struct {
  int col;
  int top;
  int bot;
  int left;
  int right;
  int curr_score;
  int high_score;
  int lives;
} Paddle;

typedef struct {
  int row;
  int col;
  int bot;
  int right;
  int up_dow;
  int left_right;
} Ball;

typedef enum {
  START,
  SETUP,
  PLAY,
  PAUSE,
  WINCLEAR,
  WIN,
  LOSECLEAR,
  LOSE,
} GBAState;

void drawPaddle(void);
void delay(int t);
Ball setBall(void);
#endif
