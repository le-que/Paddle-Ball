#include "main.h"

#include <stdio.h>
#include <stdlib.h>

#include "gba.h"
#include "images/start_background.h"
#include "images/win.h"
#include "images/game_over.h"
#include "images/birdie.h"

Ball b;
Paddle p;
char buffer[51];

int main(void) {
  REG_DISPCNT = MODE3 | BG2_ENABLE;

  //button shtuff
  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  // Load initial game state
  GBAState state = START;

  //start gba
  while (1) {
    currentButtons = BUTTONS;  // Load the current state of the buttons
    waitForVBlank();
    switch (state) {
      case START:       //start screen
        drawFullScreenImageDMA(start_background);
        drawString(80, 100, "PADDLE BALL", WHITE);
        drawString(140, 7, "Press ENTER to play...", WHITE);
        if (KEY_DOWN(BUTTON_START, currentButtons)) {
          state = SETUP;
        }
        break;
      case SETUP: 
        fillScreenDMA(BLACK);
        setBall();
        p.high_score = 0;
        p.lives = 3;
        drawPaddle();       //make draw platform
        state = PLAY;
        break;
      case PLAY:
        waitForVBlank();
        u32 buttons = BUTTONS;
        // buttons user can press
        if (KEY_DOWN(BUTTON_SELECT, buttons)) {
          fillScreenDMA(BLACK);
          state = START;
        }
        if (KEY_DOWN(BUTTON_RIGHT, buttons)) {
          drawRectDMA(140, p.col, 30, 5, BLACK); // cover old platform
          p.col += 2;
          p.left += 2;
          p.right += 2;
          if (p.right >= 240) {
            p.right = 240;
            p.left = p.right - 30;
            p.col = p.left;
          }
          drawRectDMA(140, p.col, 30, 5, WHITE); // create new platform
        }
        if (KEY_DOWN(BUTTON_LEFT, buttons)) {
          drawRectDMA(140, p.col, 30, 5, BLACK); // cover old platform
          p.col -= 2;
          p.left -= 2;
          p.right -= 2;
          if (p.col <= 0) {
            p.right = p.col + 30;
            p.left = p.col;
            p.col = 0;
          }
          drawRectDMA(140, p.col, 30, 5, WHITE); // create new platform
        }
        if (KEY_DOWN(BUTTON_A, buttons)) {
          state = PAUSE;
        }
        drawRectDMA(b.row, b.col, 4, 4, BLACK); // cover old ball
        if(b.row > 140) {
          p.lives--;
          delay(3);
          drawRectDMA(140, p.col, 30, 5, BLACK); // cover old platform
          if (p.curr_score > p.high_score) {
            p.high_score = p.curr_score;
          }
          if (p.lives == 0) {
            state = LOSECLEAR; 
          }
          b = setBall();
          drawRectDMA(b.row, b.col, 5, 5, BLACK);
          drawPaddle();
        } else if (b.row < 0) {
          b.up_dow = 1;
          b.row = 0;
        }
        if(b.col < 0) {
          b.left_right = 1;
          b.col = 0;
        }
        if(b.col > 236) {
          b.left_right = -1;
          b.col = 235;
        }
        // bounce off paddle
        if (b.row + 4 >= p.top && b.col >= p.left && b.col + 4 <= p.right) {
          b.up_dow = -1;
          b.row = p.top - 4;
          p.curr_score++;
          drawRectDMA(150, 5, 60, 8, BLACK);
          sprintf(buffer, "SCORE: %d", p.curr_score);
          drawString(150, 5, buffer, WHITE);
        } 
        b.row += b.up_dow;
        b.col += b.left_right;
        b.right = b.col + 4;
        b.bot = b.row + 4;
        drawImageDMA(b.row, b.col, 4, 4, birdie);
        delay(1);
        if (p.curr_score == 20) {
          state = WINCLEAR;
        }
        break;
      case PAUSE:
        drawRectDMA(140, p.col, 30, 5, BLACK);
        drawImageDMA(b.row, b.col, 4, 4, BLACK);
        drawRectDMA(140, p.col, 30, 5, WHITE);
        drawImageDMA(b.row, b.col, 4, 4, birdie);
        if (KEY_DOWN(BUTTON_START, currentButtons)) { 
          state = PLAY;
        } else if (KEY_DOWN(BUTTON_SELECT, currentButtons)) {
          state = START;
        }
        break;
      case WINCLEAR:
        fillScreenDMA(BLACK);
        state = WIN;
        break;
      case WIN:
        drawFullScreenImageDMA(win);
        sprintf(buffer, "SCORE: %d", p.curr_score);
        drawString(100, 97, buffer, WHITE);
        drawString(140, 7, "Press ENTER to play again...", WHITE);
        if (KEY_DOWN(BUTTON_START, currentButtons)) { 
          state = SETUP;
        } else if (KEY_DOWN(BUTTON_SELECT, currentButtons)) {
          state = START;
        }
        break;
      case LOSECLEAR:
        fillScreenDMA(BLACK);
        state = LOSE;
        break;
      case LOSE:
        drawFullScreenImageDMA(game_over);
        sprintf(buffer, "HIGH SCORE: %d", p.high_score);
        drawString(120, 97, buffer, WHITE);
        drawString(140, 7, "Press ENTER to play again...", WHITE);
        if (KEY_DOWN(BUTTON_START, currentButtons)) {
          state = SETUP;
        } else if (KEY_DOWN(BUTTON_SELECT, currentButtons)) {
          state = START;
        }
        break;
    }
    previousButtons = currentButtons;
  }
  UNUSED(previousButtons);
  return 0;
}

Ball setBall(void) {
  b.row = 135;
  b.col = 240/2;
  b.right = b.col + 4;
  b.bot = b.row + 4;
  b.up_dow = -1;
  b.left_right = ((rand() % 3) -1) == 1 ? 1 : -1;
  drawImageDMA(b.row, b.col, 4, 4, birdie);
  return b;
}
void drawPaddle(void) {
  p.curr_score = 0;
  p.col = 240/2;
  p.top = 140;
  p.bot = p.top + 5;
  p.left = 240/2;
  p.right = p.left + 30;
  drawRectDMA(150, 180, 60, 8, BLACK);
  drawRectDMA(140, p.col, 30, 5, WHITE);
  sprintf(buffer, "LIVES: %d", p.lives);
  drawString(150, 180, buffer, WHITE);
  drawRectDMA(150, 5, 60, 8, BLACK);
  sprintf(buffer, "SCORE: %d", p.curr_score);
  drawString(150, 5, buffer, WHITE);
}

void delay(int t) {  
  volatile int x = 0;
  for(int i=0; i < t * 8000; i++) {
    x++;
  }
}