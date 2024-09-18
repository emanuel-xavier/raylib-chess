#include "color.h"
#include "game.h"
#include "raylib.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 640

struct Piece *selected = NULL;
struct Game *game = NULL;

float clamp(float value, float min, float max) {
  const float t = value < min ? min : value;
  return t > max ? max : t;
}

void update() {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    TraceLog(LOG_DEBUG, "Left mouse button pressed");
    Vector2 square = GetSquareOverlabByTheCursor();
    selected = GetPieceInXYPosition(game, square.x, square.y);
    if (selected != NULL) {
      if (selected->player == GetCurrentPlayer(game)) {
        selected->pos = GetMousePosition();
        TraceLog(LOG_DEBUG, "piece on square %d-%d was selected",
                 (int)selected->square.x, (int)selected->square.y);
      } else {
        TraceLog(LOG_DEBUG, "Can't move other player's piece");
        selected = NULL;
      }
    } else {
      TraceLog(LOG_DEBUG, "No piece on the square %d-%d", (int)square.x,
               (int)square.y);
    }
  }
  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    TraceLog(LOG_DEBUG, "Left mouse button released");
    bool couldMove = false;
    if (selected != NULL) {
      Vector2 newSquare = GetSquareOverlabByTheCursor();
      TraceLog(LOG_DEBUG, "next square %f-%f", newSquare.x, newSquare.y);

      if (MovePiece(game, selected, &newSquare)) {
        TraceLog(LOG_DEBUG, "piece was released at %f-%f", selected->square.x,
                 selected->square.y);
        NextPlayer(game);
        couldMove = true;
      } else {
        TraceLog(LOG_DEBUG, "Can't move piece");
      }
      if (!couldMove) {
        TraceLog(LOG_DEBUG, "piece was released at it origial square %f-%f",
                 selected->square.x, selected->square.y);

        selected->pos.x = selected->square.x * SQUARE_SIZE;
        selected->pos.y = selected->square.y * SQUARE_SIZE;
      }
      selected = NULL;
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
      struct Piece *p = GetPieceInXYPosition(game, x, y);
      if (p != NULL) {
        unsigned padding = (SQUARE_SIZE - PIECE_IMG_SIZE) / 2;
        DrawTexture(*GetPieceTexture(p), p->pos.x + padding, p->pos.y + padding,
                    WHITE);
      }
    }
  }

  if (selected != NULL) {
    unsigned padding = (SQUARE_SIZE - PIECE_IMG_SIZE) / 2;
    DrawTexture(*GetPieceTexture(selected), selected->pos.x + padding,
                selected->pos.y + padding, WHITE);
  }

  EndDrawing();
}

int main() {

#ifdef DEBUG_MODE
  SetTraceLogLevel(LOG_DEBUG);
#endif
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Chess");
  SetTargetFPS(60);

  game = NewGame();

  while (!WindowShouldClose()) {
    update();
    draw();
  }

  CloseWindow();
  DeleteGame(game);

  return 0;
}
