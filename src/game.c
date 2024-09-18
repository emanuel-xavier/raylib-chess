#include "game.h"
#include <raylib.h>

struct Game *NewGame() {
  TraceLog(LOG_DEBUG, "Allocating game structure");
  struct Game *game = (struct Game *)malloc(sizeof(struct Game));
  if (game == NULL) {
    TraceLog(LOG_ERROR, "Failed to allocate memory for game");
    return NULL;
  }

  TraceLog(LOG_DEBUG, "Allocating board structure");
  game->board = (struct Board *)malloc(sizeof(struct Board));
  if (game->board == NULL) {
    TraceLog(LOG_ERROR, "Failed to allocate memory for board");
    free(game);
    return NULL;
  }

  TraceLog(LOG_DEBUG, "Allocating board pieces matrix");
  game->board->pieces = (struct Piece ***)malloc(8 * sizeof(struct Piece **));
  if (game->board->pieces == NULL) {
    TraceLog(LOG_ERROR, "Failed to allocate memory for pieces matrix");
    free(game->board);
    free(game);
    return NULL;
  }

  for (int i = 0; i < 8; i++) {
    game->board->pieces[i] =
        (struct Piece **)malloc(8 * sizeof(struct Piece *));
    if (game->board->pieces[i] == NULL) {
      TraceLog(LOG_ERROR,
               "Failed to allocate memory for row %d in pieces matrix", i);
      for (int k = 0; k < i; k++) {
        free(game->board->pieces[k]);
      }
      free(game->board->pieces);
      free(game->board);
      free(game);
      return NULL;
    }

    // Initialize each piece to NULL
    for (int j = 0; j < 8; j++) {
      game->board->pieces[i][j] = NULL;
    }
  }

  game->board->size = 8;

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
      DeleteGame(game);
      return NULL;
    }

    // Load black piece texture
    sprintf(filePath, filePathFormatStr, "black", i);
    Image blackPieceImg = LoadImage(filePath);
    if (blackPieceImg.data == NULL) {
      TraceLog(LOG_ERROR, "Failed to load black piece image from '%s'",
               filePath);
      UnloadImage(whitePieceImg);
      DeleteGame(game);
      return NULL;
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
      DeleteGame(game);
      return NULL;
    }

    UnloadImage(whitePieceImg);
    UnloadImage(blackPieceImg);

    _whitePieceTextures[i] = whitePieceTexture;
    _blackPieceTextures[i] = blackPieceTexture;

    TraceLog(LOG_DEBUG, "Successfully loaded textures for index %d", i);
  }

  ResetDefaultConfiguration(game);
  TraceLog(LOG_DEBUG, "Game initialization complete");

  return game;
}

void DeleteGame(struct Game *game) {
  if (game == NULL) {
    TraceLog(LOG_INFO, "Attempted to delete a NULL game pointer");
    return;
  }

  TraceLog(LOG_DEBUG, "Deleting game structure");

  // Deallocate memory for pieces
  TraceLog(LOG_DEBUG, "Deleting board pieces matrix");
  if (game->board != NULL) {
    if (game->board->pieces != NULL) {
      for (int i = 0; i < 8; i++) {
        if (game->board->pieces[i] != NULL) {
          TraceLog(LOG_DEBUG, "Freeing row %d of pieces matrix", i);
          free(game->board->pieces[i]);
        }
      }
      free(game->board->pieces);
    }
    free(game->board);
  }

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

  TraceLog(LOG_DEBUG, "Freeing game structure");
  free(game);

  TraceLog(LOG_DEBUG, "Game deletion complete");
}

void resetPlayerPieces(struct Game *game, enum Player player,
                       struct Piece *pieces) {
  int x = 0, y;
  int backRowY = (player == WhitePlayer) ? 7 : 0;
  int pawnRowY = (player == WhitePlayer) ? 6 : 1;
  int piecesOrder[3] = {Rook, Knight, Bishop};

  TraceLog(LOG_DEBUG, "Placing pawns for player %d", player);
  // Place pawns
  for (x = 0; x < 8; x++) {
    pieces[x] = (struct Piece){
        .player = player,
        .type = Pawn,
        .square = (Vector2){x, pawnRowY},
    };
    game->board->pieces[x][pawnRowY] = &pieces[x];
    TraceLog(LOG_DEBUG, "Placed pawn at (%d, %d)", x, pawnRowY);
  }

  TraceLog(LOG_DEBUG, "Initializing rooks, knights, and bishops for player %d",
           player);
  // Initialize rooks, knights, and bishops
  for (y = 0; y < 3; y++) {
    for (int k = 0; k < 2; k++, x++) {
      int yPos = (k == 0) ? y : 7 - y;
      pieces[x] = (struct Piece){
          .player = player,
          .type = piecesOrder[y],
          .square = (Vector2){yPos, backRowY},
      };
      game->board->pieces[yPos][backRowY] = &pieces[x];
      TraceLog(LOG_DEBUG, "Placed %d at (%d, %d)", piecesOrder[y], yPos,
               backRowY);
    }
  }

  TraceLog(LOG_DEBUG, "Placing king for player %d", player);
  // Place king
  pieces[x] = (struct Piece){
      .player = player,
      .type = King,
      .square = (Vector2){4, backRowY},
  };
  game->board->pieces[4][backRowY] = &pieces[x];
  TraceLog(LOG_DEBUG, "Placed king at (4, %d)", backRowY);

  TraceLog(LOG_DEBUG, "Placing queen for player %d", player);
  // Place queen
  x++;
  pieces[x] = (struct Piece){
      .player = player,
      .type = Queen,
      .square = (Vector2){3, backRowY},
  };
  game->board->pieces[3][backRowY] = &pieces[x];
  TraceLog(LOG_DEBUG, "Placed queen at (3, %d)", backRowY);
}

void ResetDefaultConfiguration(struct Game *game) {
  int x, y;

  TraceLog(LOG_DEBUG, "Initializing empty squares");
  // Initialize empty squares
  for (y = 0; y < 8; y++) {
    for (x = 0; x < 8; x++) {
      game->board->pieces[y][x] = NULL;
    }
  }

  TraceLog(LOG_DEBUG, "Initializing white pieces");
  resetPlayerPieces(game, WhitePlayer, game->_whitePieces);

  TraceLog(LOG_DEBUG, "Initializing black pieces");
  resetPlayerPieces(game, BlackPlayer, game->_blackPieces);

  TraceLog(LOG_DEBUG, "Setting piece positions");
  // Set positions for all pieces
  for (x = 0; x < 8; x++) {
    for (y = 0; y < 2; y++) {
      if (game->board->pieces[x][y] != NULL) {
        game->board->pieces[x][y]->pos = (Vector2){
            .x = game->board->pieces[x][y]->square.x * (float)SQUARE_SIZE,
            .y = game->board->pieces[x][y]->square.y * (float)SQUARE_SIZE,
        };
        TraceLog(LOG_DEBUG, "Set position for piece at (%d, %d)", x, y);
      }
    }
    for (y = 6; y < 8; y++) {
      if (game->board->pieces[x][y] != NULL) {
        game->board->pieces[x][y]->pos = (Vector2){
            .x = game->board->pieces[x][y]->square.x * (float)SQUARE_SIZE,
            .y = game->board->pieces[x][y]->square.y * (float)SQUARE_SIZE,
        };
        TraceLog(LOG_DEBUG, "Set position for piece at (%d, %d)", x, y);
      }
    }
  }

  TraceLog(LOG_DEBUG, "Setting current player to WhitePlayer");
  game->_currentPlayer = WhitePlayer;

  TraceLog(LOG_DEBUG, "Game reset complete");
}

struct Piece *GetPieceInXYPosition(const struct Game *game, unsigned x,
                                   unsigned y) {
  return game->board->pieces[x][y];
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

enum Player GetCurrentPlayer(const struct Game *game) {
  return game->_currentPlayer;
}

enum Player NextPlayer(struct Game *game) {
  TraceLog(LOG_DEBUG, "Current player: %d", game->_currentPlayer);

  if (game->_currentPlayer == WhitePlayer) {
    game->_currentPlayer = BlackPlayer;
    TraceLog(LOG_DEBUG, "Switching to BlackPlayer");
  } else {
    game->_currentPlayer = WhitePlayer;
    TraceLog(LOG_DEBUG, "Switching to WhitePlayer");
  }

  TraceLog(LOG_DEBUG, "Next player: %d", game->_currentPlayer);
  return game->_currentPlayer;
}

void MovePiece(struct Game *game, struct Piece *piece, const Vector2 *pos) {
  Vector2 oldPos = piece->square;

  TraceLog(LOG_DEBUG, "Moving piece from (%d, %d) to (%d, %d)", (int)oldPos.x,
           (int)oldPos.y, (int)pos->x, (int)pos->y);

  piece->square.x = pos->x;
  piece->square.y = pos->y;

  piece->pos.x = pos->x * SQUARE_SIZE;
  piece->pos.y = pos->y * SQUARE_SIZE;

  // Update board with new position
  TraceLog(
      LOG_DEBUG,
      "Updating board position: setting (%d, %d) to piece and (%d, %d) to NULL",
      (int)pos->x, (int)pos->y, (int)oldPos.x, (int)oldPos.y);

  game->board->pieces[(unsigned)pos->x][(unsigned)pos->y] = piece;
  game->board->pieces[(unsigned)oldPos.x][(unsigned)oldPos.y] = NULL;

  TraceLog(LOG_DEBUG, "Piece moved to new position");
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
