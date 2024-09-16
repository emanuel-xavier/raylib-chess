#include "game.h"

void Init() {
  _board.pieces = (struct Piece ***)malloc(8 * sizeof(struct Piece **));
  if (_board.pieces == NULL) {
    printf("Memory allocation failed - pieces matrix\n");
  }

  for (int i = 0; i < 8; i++) {
    _board.pieces[i] = (struct Piece **)malloc(8 * sizeof(struct Piece *));
    if (_board.pieces[i] == NULL) {
      printf("Memory allocation failed - row(%d)\n", i);
      for (int j = 0; j < i; j++) {
        free(_board.pieces[j]);
      }
      free(_board.pieces);
    }
  }

  _board.size = 8;

  Reset();
}

void Deinit() {
  if (_board.pieces != NULL) {
    for (int i = 0; i < 8; i++) {
      if (_board.pieces[i] != NULL) {
        for (int j = 0; j < 8; j++) {
          if (_board.pieces[i][j] != NULL)
            free(_board.pieces[i][j]);
        }
        free(_board.pieces[i]);
      }
    }
    free(_board.pieces);
  }
}

void Reset() {
  int i, j;
  enum Player blackPlayer = BlackPlayer;
  enum Player whitePlayer = WhitePlayer;

  // Initialize empty squares
  for (i = 2; i < 6; i++) {
    for (j = 0; j < 8; j++) {
      _board.pieces[i][j] = NULL;
    }
  }

  // Load piece's textures once
  for (i = 0; i < 6; i++) {
    printf("Loading piece img\n");
    char *filePathFormatStr = "src/resources/pieces/%s/%i.png";
    char filePath[100];

    sprintf(filePath, filePathFormatStr, "white", i);
    Image whitePieceImg = LoadImage(filePath);
    sprintf(filePath, filePathFormatStr, "black", i);
    Image blackPieceImg = LoadImage(filePath);
    printf("img loaded\nLoading texture\n");

    Texture2D whitePieceTexture = LoadTextureFromImage(whitePieceImg);
    Texture2D blackPieceTexture = LoadTextureFromImage(blackPieceImg);
    printf("texture loaded\n");

    while (!IsTextureReady(whitePieceTexture) ||
           !IsTextureReady(blackPieceTexture)) {
      printf("Waiting pawn textures to be ready\n");
      usleep(1000 * 100);
    }
    printf("Textures are ready\n");

    _whitePieceTextures[i] = whitePieceTexture;
    _blackPieceTextures[i] = blackPieceTexture;

    _whitePieces[i] = (struct Piece){whitePlayer, i, &whitePieceTexture};
    _blackPieces[i] = (struct Piece){blackPlayer, i, &blackPieceTexture};
  }

  // Reset board positions
  // place pawns
  for (j = 0; j < 8; j++) {
    _board.pieces[1][j] = &_whitePieces[Pawn];
    _board.pieces[6][j] = &_blackPieces[Pawn];
  }
  // place rooks
  _board.pieces[0][0] = &_whitePieces[Rook];
  _board.pieces[0][7] = &_whitePieces[Rook];
  _board.pieces[7][0] = &_blackPieces[Rook];
  _board.pieces[7][7] = &_blackPieces[Rook];

  // place knights
  _board.pieces[0][1] = &_whitePieces[Knight];
  _board.pieces[0][6] = &_whitePieces[Knight];
  _board.pieces[7][1] = &_blackPieces[Knight];
  _board.pieces[7][6] = &_blackPieces[Knight];

  // place bishops
  _board.pieces[0][2] = &_whitePieces[Bishop];
  _board.pieces[0][5] = &_whitePieces[Bishop];
  _board.pieces[7][2] = &_blackPieces[Bishop];
  _board.pieces[7][5] = &_blackPieces[Bishop];

  // king and queens
  _board.pieces[0][4] = &_whitePieces[King];
  _board.pieces[0][3] = &_whitePieces[Queen];
  _board.pieces[7][4] = &_blackPieces[King];
  _board.pieces[7][3] = &_blackPieces[Queen];
}

struct Piece *GetPieceInXYPosition(unsigned x, unsigned y) {
  return _board.pieces[x][y];
}
