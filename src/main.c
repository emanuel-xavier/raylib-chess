#include "color.h"
#include "game.h"
#include "raylib.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 640

struct Piece *selected = NULL;
Vector2 selectedOldPosition = {-1, -1};

float clamp(float value, float min, float max) {
  const float t = value < min ? min : value;
  return t > max ? max : t;
}

void update() {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 square = GetSquareOverlabByTheCursor();
    selected = GetPieceInXYPosition(square.x, square.y);
    if (selected != NULL) {
      if (selected->player == GetCurrentPlayer()) {
        selectedOldPosition = selected->square;
        selected->pos = GetMousePosition();
        printf("piece on square %f-%f was selected\n", selected->square.x,
               selected->square.y);
      } else {
        selected = NULL;
      }
    }
  }
  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    if (selected != NULL) {
      if (selected->square.x != selectedOldPosition.x ||
          selected->square.y != selectedOldPosition.y) {
        Vector2 newSquare = GetSquareOverlabByTheCursor();
        // _board.pieces[(int)selectedOldPosition.x][(int)selectedOldPosition.y]
        // =
        //     NULL;
        // _board.pieces[(int)newSquare.x][(int)newSquare.y] = selected;
        selected->square = newSquare;
        selected->pos.x = newSquare.x * SQUARE_SIZE;
        selected->pos.y = newSquare.y * SQUARE_SIZE;
        printf("piece was released at %f-%f\n", selected->square.x,
               selected->square.y);
        selected = NULL;
        NextPlayer();
      } else {
        printf("piece was released at it origial square %f-%f\n",
               selected->square.x, selected->square.y);

        selected->square = selectedOldPosition;
        selected->pos.x = selectedOldPosition.x * SQUARE_SIZE;
        selected->pos.y = selectedOldPosition.y * SQUARE_SIZE;
      }
    }
  }

  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    if (selected != NULL) {
      Vector2 mousePos = GetMousePosition();
      mousePos.x =
          clamp(mousePos.x, 0, WINDOW_WIDTH) - (float)PIECE_IMG_SIZE / 2;
      mousePos.y =
          clamp(mousePos.y, 0, WINDOW_WIDTH) - (float)PIECE_IMG_SIZE / 2;

      selected->pos = mousePos;
      selected->square = GetSquareOverlabByTheCursor();
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
    }
  }

  for (int y = 0; y < BOARD_SIZE; y++) {
    for (int x = 0; x < BOARD_SIZE; x++) {
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

  while (!WindowShouldClose()) {
    update();
    draw();
  }

  CloseWindow();
  Deinit();

  return 0;
}
