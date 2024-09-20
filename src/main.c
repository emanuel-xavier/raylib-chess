#include "color.h"
#include "game.h"
#include "raylib.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 640

struct Piece *selected = NULL;
struct Game *game = NULL;

static Texture2D _whitePieceTextures[6];
static Texture2D _blackPieceTextures[6];

void LoadGameTextures() {
  // Load piece textures
  for (int i = 0; i < 6; i++) {
    char *filePathFormatStr = "src/resources/pieces/%s/%i.png";
    char filePath[100];

    const char *app_dir = GetApplicationDirectory();
    ChangeDirectory(app_dir);

    TraceLog(LOG_DEBUG, "Loading piece texture for index %d", i);

    // Load white piece texture
    sprintf(filePath, filePathFormatStr, "white", i);
    Image whitePieceImg = LoadImage(filePath);
    if (whitePieceImg.data == NULL) {
      TraceLog(LOG_ERROR, "Failed to load white piece image from '%s'",
               filePath);
    }

    // Load black piece texture
    sprintf(filePath, filePathFormatStr, "black", i);
    Image blackPieceImg = LoadImage(filePath);
    if (blackPieceImg.data == NULL) {
      TraceLog(LOG_ERROR, "Failed to load black piece image from '%s'",
               filePath);
      UnloadImage(whitePieceImg);
    }

    // Resize and load textures
    ImageResize(&whitePieceImg, PIECE_IMG_SIZE, PIECE_IMG_SIZE);
    ImageResize(&blackPieceImg, PIECE_IMG_SIZE, PIECE_IMG_SIZE);
    Texture2D whitePieceTexture = LoadTextureFromImage(whitePieceImg);
    Texture2D blackPieceTexture = LoadTextureFromImage(blackPieceImg);

    if (!IsTextureReady(whitePieceTexture) ||
        !IsTextureReady(blackPieceTexture)) {
      TraceLog(LOG_ERROR, "Failed to load textures properly");
      UnloadImage(whitePieceImg);
      UnloadImage(blackPieceImg);
    }

    UnloadImage(whitePieceImg);
    UnloadImage(blackPieceImg);

    _whitePieceTextures[i] = whitePieceTexture;
    _blackPieceTextures[i] = blackPieceTexture;

    TraceLog(LOG_DEBUG, "Successfully loaded textures for index %d", i);
  }
}

Vector2 GetSquareOverlabByTheCursor() {
  float mouseX = GetMouseX();
  float mouseY = GetMouseY();

  Vector2 pos = (Vector2){
      .x = (int)(mouseX / SQUARE_SIZE),
      .y = (int)(mouseY / SQUARE_SIZE),
  };

  TraceLog(LOG_DEBUG,
           "Raw Mouse position: (%d, %d) - Square position: (%d, %d)",
           (int)mouseX, (int)mouseY, (int)pos.x, (int)pos.y);
  return pos;
}

void UnloadGameTextures() {
  // Unload textures
  TraceLog(LOG_DEBUG, "Unloading textures");
  for (int i = 0; i < 6; i++) {
    if (IsTextureReady(_whitePieceTextures[i])) {
      UnloadTexture(_whitePieceTextures[i]);
      _whitePieceTextures[i] = (Texture2D){0};
    }
    if (IsTextureReady(_blackPieceTextures[i])) {
      UnloadTexture(_blackPieceTextures[i]);
      _blackPieceTextures[i] = (Texture2D){0};
    }
  }
}

Texture2D *GetPieceTexture(const struct Piece *piece) {
  if (piece->player == WhitePlayer)
    return &(_whitePieceTextures[piece->type]);
  return &(_blackPieceTextures[piece->type]);
}

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
  LoadGameTextures();

  while (!WindowShouldClose()) {
    update();
    draw();
  }

  CloseWindow();

  DeleteGame(game);
  UnloadGameTextures();

  return 0;
}
