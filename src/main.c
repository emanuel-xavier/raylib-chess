#include "color.h"
#include "game.h"
#include "raylib.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 640
#define SQUARE_SIZE 80
#define BOARD_SIZE 8

struct Board *board = NULL;

void update() {}

void draw() {

  Color squareColor;

  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      if ((i + j) % 2 == 0) {
        squareColor = green;
      } else {
        squareColor = white;
      }

      int xPos = SQUARE_SIZE * i, yPos = SQUARE_SIZE * j;

      DrawRectangle(xPos, yPos, SQUARE_SIZE, SQUARE_SIZE, squareColor);

      printf("drawing piece on position %d-%d\n", i, j);
      if (board->pieces[i][j] != NULL) {
        struct Piece *p = GetPieceInXYPosition(i, j);

        if (p != NULL) {
          printf("Drawning pawn on position %d-%d\n", i, j);
          DrawTexture(*p->img, xPos, yPos, white);
        }
      }
    }
  }
  EndDrawing();
}

int main() {

  Init();
  Reset();

  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Chess");

  while (!WindowShouldClose()) {
    update();
    draw();
  }

  CloseWindow();
  Deinit();

  return 0;
}
