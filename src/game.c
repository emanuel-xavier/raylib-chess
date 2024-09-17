#include "game.h"

void Init() {
  // Allocate memory for pieces matrix
  _board.pieces = (struct Piece ***)malloc(8 * sizeof(struct Piece **));
  if (_board.pieces == NULL) {
    printf("Memory allocation failed - pieces matrix\n");
    exit(1);
  }

  for (int i = 0; i < 8; i++) {
    _board.pieces[i] = (struct Piece **)malloc(8 * sizeof(struct Piece *));
    if (_board.pieces[i] == NULL) {
      printf("Memory allocation failed - row(%d)\n", i);
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
    printf("Loading piece img for index %d\n", i);

    // Load white piece texture
    sprintf(filePath, filePathFormatStr, "white", i);
    Image whitePieceImg = LoadImage(filePath);
    if (whitePieceImg.data == NULL) {
      printf("Failed to load image from '%s'\n", filePath);
      Deinit();
      exit(1);
    }

    // Load black piece texture
    sprintf(filePath, filePathFormatStr, "black", i);
    Image blackPieceImg = LoadImage(filePath);
    if (blackPieceImg.data == NULL) {
      printf("Failed to load image from '%s'\n", filePath);
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
      printf("Texture is not ready\n");
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

void Reset() {
  int x, y;

  // Initialize empty squares
  for (y = 0; y < 8; y++) {
    for (x = 0; x < 8; x++) {
      _board.pieces[y][x] = NULL;
    }
  }

  // Place pawns
  printf("Initializing pawns\n");
  for (x = 0; x < 8; x++) {
    _whitePieces[x] = (struct Piece){
        .player = WhitePlayer,
        .type = Pawn,
        .square = (Vector2){x, 6},
    };
    _blackPieces[x] = (struct Piece){
        .player = BlackPlayer,
        .type = Pawn,
        .square = (Vector2){x, 1},
    };
    _board.pieces[x][6] = &_whitePieces[x];
    _board.pieces[x][1] = &_blackPieces[x];
  }

  // Place rooks
  printf("Initializing rooks\n");
  _whitePieces[x] = (struct Piece){
      .player = WhitePlayer,
      .type = Rook,
      .square = (Vector2){0, 7},
  };
  _board.pieces[0][7] = &_whitePieces[x];

  _blackPieces[x] = (struct Piece){
      .player = BlackPlayer,
      .type = Rook,
      .square = (Vector2){0, 0},
  };
  _board.pieces[0][0] = &_blackPieces[x];

  x++;
  _whitePieces[x] = (struct Piece){
      .player = WhitePlayer,
      .type = Rook,
      .square = (Vector2){7, 7},
  };
  _board.pieces[7][7] = &_whitePieces[x];

  _blackPieces[x] = (struct Piece){
      .player = BlackPlayer,
      .type = Rook,
      .square = (Vector2){7, 0},
  };
  _board.pieces[7][0] = &_blackPieces[x];

  // Place knights
  printf("Initializing knights\n");
  x++;
  _whitePieces[x] = (struct Piece){
      .player = WhitePlayer,
      .type = Knight,
      .square = (Vector2){1, 7},
  };
  _board.pieces[1][7] = &_whitePieces[x];

  _blackPieces[x] = (struct Piece){
      .player = BlackPlayer,
      .type = Knight,
      .square = (Vector2){1, 0},
  };
  _board.pieces[1][0] = &_blackPieces[x];

  x++;
  _whitePieces[x] = (struct Piece){
      .player = WhitePlayer,
      .type = Knight,
      .square = (Vector2){6, 7},
  };
  _board.pieces[6][7] = &_whitePieces[x];

  _blackPieces[x] = (struct Piece){
      .player = BlackPlayer,
      .type = Knight,
      .square = (Vector2){6, 0},
  };
  _board.pieces[6][0] = &_blackPieces[x];

  // Place bishops
  printf("Initializing bishops\n");
  x++;
  _whitePieces[x] = (struct Piece){
      .player = WhitePlayer,
      .type = Bishop,
      .square = (Vector2){2, 7},
  };
  _board.pieces[2][7] = &_whitePieces[x];

  _blackPieces[x] = (struct Piece){
      .player = BlackPlayer,
      .type = Bishop,
      .square = (Vector2){2, 0},
  };
  _board.pieces[2][0] = &_blackPieces[x];

  x++;
  _whitePieces[x] = (struct Piece){
      .player = WhitePlayer,
      .type = Bishop,
      .square = (Vector2){5, 7},
  };
  _board.pieces[5][7] = &_whitePieces[x];

  _blackPieces[x] = (struct Piece){
      .player = BlackPlayer,
      .type = Bishop,
      .square = (Vector2){5, 0},
  };
  _board.pieces[5][0] = &_blackPieces[x];

  // Place kings
  printf("Initializing kings\n");
  x++;
  _whitePieces[x] = (struct Piece){
      .player = WhitePlayer,
      .type = King,
      .square = (Vector2){4, 7},
  };
  _board.pieces[4][7] = &_whitePieces[x];

  _blackPieces[x] = (struct Piece){
      .player = BlackPlayer,
      .type = King,
      .square = (Vector2){4, 0},
  };
  _board.pieces[4][0] = &_blackPieces[x];

  // Place queens
  printf("Initializing queens\n");
  x++;
  _whitePieces[x] = (struct Piece){
      .player = WhitePlayer,
      .type = Queen,
      .square = (Vector2){3, 7},
  };
  _board.pieces[3][7] = &_whitePieces[x];

  _blackPieces[x] = (struct Piece){
      .player = BlackPlayer,
      .type = Queen,
      .square = (Vector2){3, 0},
  };
  _board.pieces[3][0] = &_blackPieces[x];

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

  printf("Board reset\n");
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

Texture2D *getPieceTexture(const struct Piece *piece) {
  if (piece->player == WhitePlayer)
    return &(_whitePieceTextures[piece->type]);
  return &(_blackPieceTextures[piece->type]);
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
