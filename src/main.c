#include "color.h"
#include "game.h"
#include "raylib.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 640

struct Piece *selected = NULL;

void update() {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 square = GetSquareOverlabByTheCursor();
    selected = GetPieceInXYPosition(square.x, square.y);
    if (selected != NULL) {
      selected->pos = GetMousePosition();
      printf("piece on square %f-%f was selected\n", selected->square.x,
             selected->square.y);
    }
  }
  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    if (selected != NULL) {
      selected->pos = GetSquareOverlabByTheCursor();
      selected->square = selected->pos;
      selected->pos.x *= SQUARE_SIZE;
      selected->pos.y *= SQUARE_SIZE;

      printf("piece on square was released at %f-%f\n", selected->square.x,
             selected->square.y);

      selected = NULL;
    }
  }

  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    if (selected != NULL) {
      selected->pos = GetSquareOverlabByTheCursor();
      selected->square = selected->pos;
      selected->pos.x *= SQUARE_SIZE;
      selected->pos.y *= SQUARE_SIZE;
      printf("dragging %f-%f\n", selected->pos.x, selected->pos.y);
    }
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

  if (selected != NULL) {
    unsigned padding = (SQUARE_SIZE - PIECE_IMG_SIZE) / 2;
    DrawTexture(*getPieceTexture(selected), selected->pos.x + padding,
                selected->pos.y + padding, WHITE);
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
