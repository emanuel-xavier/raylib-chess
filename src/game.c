#include "game.h"

void Init() {
  // Allocate memory for pieces matrix
  _board.pieces = (struct Piece ***)malloc(8 * sizeof(struct Piece **));
  if (_board.pieces == NULL) {
    TraceLog(LOG_DEBUG, "Memory allocation failed - pieces matrix");
    exit(1);
  }

  for (int i = 0; i < 8; i++) {
    _board.pieces[i] = (struct Piece **)malloc(8 * sizeof(struct Piece *));
    if (_board.pieces[i] == NULL) {
      TraceLog(LOG_DEBUG, "Memory allocation failed - row(%d)", i);
      for (int j = 0; j < i; j++) {
        free(_board.pieces[j]);
      }
      free(_board.pieces);
      exit(1);
    }

    // Initialize each piece to NULL
    for (int j = 0; j < 8; j++) {
      _board.pieces[i][j] = NULL;
    }
  }

  _board.size = 8;

  // Load piece textures
  for (int i = 0; i < 6; i++) {
    char *filePathFormatStr = "src/resources/pieces/%s/%i.png";
    char filePath[100];

    const char *app_dir = GetApplicationDirectory();
    ChangeDirectory(app_dir);
    TraceLog(LOG_DEBUG, "Loading piece img for index %d", i);

    // Load white piece texture
    sprintf(filePath, filePathFormatStr, "white", i);
    Image whitePieceImg = LoadImage(filePath);
    if (whitePieceImg.data == NULL) {
      TraceLog(LOG_DEBUG, "Failed to load image from '%s'", filePath);
      Deinit();
      exit(1);
    }

    // Load black piece texture
    sprintf(filePath, filePathFormatStr, "black", i);
    Image blackPieceImg = LoadImage(filePath);
    if (blackPieceImg.data == NULL) {
      TraceLog(LOG_DEBUG, "Failed to load image from '%s'", filePath);
      UnloadImage(
          whitePieceImg); // Unload the white image since it was already loaded
      Deinit();
      exit(1);
    }

    // Resize and load textures
    ImageResize(&whitePieceImg, PIECE_IMG_SIZE, PIECE_IMG_SIZE);
    ImageResize(&blackPieceImg, PIECE_IMG_SIZE, PIECE_IMG_SIZE);
    Texture2D whitePieceTexture = LoadTextureFromImage(whitePieceImg);
    Texture2D blackPieceTexture = LoadTextureFromImage(blackPieceImg);

    if (!IsTextureReady(whitePieceTexture) ||
        !IsTextureReady(blackPieceTexture)) {
      TraceLog(LOG_DEBUG, "Texture is not ready");
      UnloadImage(whitePieceImg);
      UnloadImage(blackPieceImg);
      Deinit();
      exit(1);
    }

    UnloadImage(whitePieceImg);
    UnloadImage(blackPieceImg);

    _whitePieceTextures[i] = whitePieceTexture;
    _blackPieceTextures[i] = blackPieceTexture;
  }

  Reset();
}

void Deinit() {
  // Deallocate memory for pieces
  if (_board.pieces != NULL) {
    for (int i = 0; i < 8; i++) {
      if (_board.pieces[i] != NULL) {
        free(_board.pieces[i]);
        _board.pieces[i] = NULL;
      }
    }
    free(_board.pieces);
    _board.pieces = NULL;
  }

  // Unload textures
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

void resetPlayerPieces(enum Player player, struct Piece *pieces) {
  int x = 0, y;
  int backRowY = (player == WhitePlayer) ? 7 : 0;
  int pawnRowY = (player == WhitePlayer) ? 6 : 1;
  int piecesOrder[3] = {Rook, Knight, Bishop};

  // Place pawns
  for (x = 0; x < 8; x++) {
    pieces[x] = (struct Piece){
        .player = player,
        .type = Pawn,
        .square = (Vector2){x, pawnRowY},
    };
    _board.pieces[x][pawnRowY] = &pieces[x];
  }

  // Initialize rooks, knights, and bishops
  for (y = 0; y < 3; y++) {
    for (int k = 0; k < 2; k++, x++) {
      int yPos = (k == 0) ? y : 7 - y;
      pieces[x] = (struct Piece){
          .player = player,
          .type = piecesOrder[y],
          .square = (Vector2){yPos, backRowY},
      };
      _board.pieces[yPos][backRowY] = &pieces[x];
    }
  }

  // Place king
  pieces[x] = (struct Piece){
      .player = player,
      .type = King,
      .square = (Vector2){4, backRowY},
  };
  _board.pieces[4][backRowY] = &pieces[x];

  // Place queen
  x++;
  pieces[x] = (struct Piece){
      .player = player,
      .type = Queen,
      .square = (Vector2){3, backRowY},
  };
  _board.pieces[3][backRowY] = &pieces[x];
}

void Reset() {
  int x, y;

  // Initialize empty squares
  for (y = 0; y < 8; y++) {
    for (x = 0; x < 8; x++) {
      _board.pieces[y][x] = NULL;
    }
  }

  TraceLog(LOG_DEBUG, "Initializing white pieces");
  resetPlayerPieces(WhitePlayer, _whitePieces);

  TraceLog(LOG_DEBUG, "Initializing black pieces");
  resetPlayerPieces(BlackPlayer, _blackPieces);

  // Set positions for all pieces
  TraceLog(LOG_DEBUG, "Setting piece positions");
  for (x = 0; x < 8; x++) {
    for (y = 0; y < 2; y++) {
      _board.pieces[x][y]->pos = (Vector2){
          .x = _board.pieces[x][y]->square.x * (float)SQUARE_SIZE,
          .y = _board.pieces[x][y]->square.y * (float)SQUARE_SIZE,
      };
    }
    for (y = 6; y < 8; y++) {
      _board.pieces[x][y]->pos = (Vector2){
          .x = _board.pieces[x][y]->square.x * (float)SQUARE_SIZE,
          .y = _board.pieces[x][y]->square.y * (float)SQUARE_SIZE,
      };
    }
  }

  TraceLog(LOG_DEBUG, "Board reset");
}

struct Piece *GetPieceInXYPosition(unsigned x, unsigned y) {
  return _board.pieces[x][y];
}

Vector2 GetSquareOverlabByTheCursor() {
  return (Vector2){
      .x = (int)(GetMouseX() / SQUARE_SIZE),
      .y = (int)(GetMouseY() / SQUARE_SIZE),
  };
}

Texture2D *GetPieceTexture(const struct Piece *piece) {
  if (piece->player == WhitePlayer)
    return &(_whitePieceTextures[piece->type]);
  return &(_blackPieceTextures[piece->type]);
}

enum Player GetCurrentPlayer() { return _currentPlayer; }

enum Player NextPlayer() {
  if (_currentPlayer == WhitePlayer)
    _currentPlayer = BlackPlayer;
  else
    _currentPlayer = WhitePlayer;

  return _currentPlayer;
}

// struct Moves GetPossibleMoves(struct Piece *piece) {
//   if (piece == NULL)
//     return (struct Moves){.size = 0};
//
//   switch (piece->type) {
//   case Pawn: {
//     if (piece->player == WhitePlayer) {
//
//     } else {
//     }
//
//     break;
//   }
//   }
// }
