#include "color.h"
#include "game.h"
#include "raylib.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 640

struct Board *board = NULL;

void update() {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 square = GetClickedSquare();
    printf("square click event %d-%d\n", (int)square.x, (int)square.y);
  }
}

void draw() {
  BeginDrawing();
  ClearBackground(WHITE);

  for (int y = 0; y < BOARD_SIZE; y++) {
    for (int x = 0; x < BOARD_SIZE; x++) {
      // Calculate the position based on column (x) and row (y)
      int xPos = SQUARE_SIZE * x;
      int yPos = SQUARE_SIZE * y;

      // Draw green squares
      if ((x + y) % 2 == 0) {
        DrawRectangle(xPos, yPos, SQUARE_SIZE, SQUARE_SIZE, BoardSquareGreen);
      }

      // Get the piece in this position and draw it
      struct Piece *p = GetPieceInXYPosition(x, y);
      if (p != NULL) {
        unsigned padding = (SQUARE_SIZE - PIECE_IMG_SIZE) / 2;
        DrawTexture(*getPieceTexture(p), p->pos.x + padding, p->pos.y + padding,
                    WHITE);
      }
    }
  }

  EndDrawing();
}

int main() {

  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Chess");
  SetTargetFPS(60);
  Init();
  Reset();

  while (!WindowShouldClose()) {
    update();
    draw();
  }

  CloseWindow();
  Deinit();

  return 0;
}
