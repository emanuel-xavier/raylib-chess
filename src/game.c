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
        .moveCounter = 0,
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

  game->moves = (struct Moves){.size = 0};

  TraceLog(LOG_DEBUG, "Game reset complete");
}

struct Piece *GetPieceInXYPosition(const struct Game *game, unsigned x,
                                   unsigned y) {
  return game->board->pieces[x][y];
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

Texture2D *GetPieceTexture(const struct Piece *piece) {
  if (piece->player == WhitePlayer)
    return &(_whitePieceTextures[piece->type]);
  return &(_blackPieceTextures[piece->type]);
}

enum Player GetCurrentPlayer(const struct Game *game) {
  return game->_currentPlayer;
}

enum Player NextPlayer(struct Game *game) {

  if (game->_currentPlayer == WhitePlayer) {
    game->_currentPlayer = BlackPlayer;
    TraceLog(LOG_DEBUG, "Switching to BlackPlayer");
  } else {
    game->_currentPlayer = WhitePlayer;
    TraceLog(LOG_DEBUG, "Switching to WhitePlayer");
  }

  TraceLog(LOG_DEBUG, "Current player: %s",
           (game->_currentPlayer == WhitePlayer ? "White" : "Black"));
  return game->_currentPlayer;
}

#include <stdio.h>

bool MovePiece(struct Game *game, struct Piece *piece, const Vector2 *pos) {
  if (game == NULL || piece == NULL || pos == NULL) {
    TraceLog(LOG_ERROR, "Invalid parameters passed to MovePiece");
    return false;
  }

  Vector2 oldPos = piece->square;

  if (oldPos.x == pos->x && oldPos.y == pos->y) {
    TraceLog(LOG_DEBUG, "Cant move the piece the it current position");
    return false;
  }
  TraceLog(LOG_DEBUG, "Moving piece from (%d, %d) to (%d, %d)", (int)oldPos.x,
           (int)oldPos.y, (int)pos->x, (int)pos->y);

  // Ensure new and old positions are within bounds
  if ((unsigned)pos->x >= game->board->size ||
      (unsigned)pos->y >= game->board->size ||
      (unsigned)oldPos.x >= game->board->size ||
      (unsigned)oldPos.y >= game->board->size) {
    TraceLog(LOG_ERROR, "Move out of bounds: old (%d, %d) new (%d, %d)",
             (int)oldPos.x, (int)oldPos.y, (int)pos->x, (int)pos->y);
    return false;
  }

  game->moves.size = 0;
  GetPossibleMoves(&game->moves, game->board, piece);
  bool validMove = false;
  for (int i = 0; i < game->moves.size; i++) {
    if (game->moves.squares[i].x == pos->x &&
        game->moves.squares[i].y == pos->y) {
      TraceLog(LOG_DEBUG, "Valid move");
      validMove = true;
      break;
    }
  }
  if (!validMove) {
    TraceLog(LOG_DEBUG, "Invalid moves");
    return false;
  }

  // Update piece position
  piece->square = *pos;
  piece->pos.x = pos->x * SQUARE_SIZE;
  piece->pos.y = pos->y * SQUARE_SIZE;

#ifdef PRINT_BOARD
  PrintFormattedBoard(game);
#endif

  game->board->pieces[(unsigned)pos->x][(unsigned)pos->y] = piece;
  game->board->pieces[(unsigned)oldPos.x][(unsigned)oldPos.y] = NULL;

#ifdef PRINT_BOARD
  PrintFormattedBoard(game);
#endif

  TraceLog(LOG_DEBUG, "Piece moved to new position: (%d, %d)", (int)pos->x,
           (int)pos->y);

  piece->moveCounter++;
  return true;
}

// Function to get character representation for a piece
char getPieceChar(const struct Piece *piece) {
  if (piece == NULL)
    return '.';

  char playerChar =
      (piece->player == WhitePlayer) ? WHITE_PLAYER : BLACK_PLAYER;
  char typeChar;

  switch (piece->type) {
  case Pawn:
    typeChar = PAWN;
    break;
  case Rook:
    typeChar = ROOK;
    break;
  case Knight:
    typeChar = KNIGHT;
    break;
  case Bishop:
    typeChar = BISHOP;
    break;
  case Queen:
    typeChar = QUEEN;
    break;
  case King:
    typeChar = KING;
    break;
  default:
    typeChar = '.';
    break;
  }

  // Combine player and type into a single character for printing
  return (piece->player == WhitePlayer)
             ? typeChar
             : typeChar + 32; // Lowercase for BlackPlayer
}

// Function to print the formatted board
void PrintFormattedBoard(const struct Game *game) {
  if (game == NULL || game->board == NULL || game->board->pieces == NULL) {
    printf("Invalid game state\n");
    return;
  }

  printf("  a b c d e f g h\n"); // Column labels
  for (int y = 7; y >= 0; y--) {
    printf("%d ", y + 1); // Row labels
    for (int x = 0; x < 8; x++) {
      char pieceChar = getPieceChar(game->board->pieces[x][y]);
      printf("%c ", pieceChar);
    }
    printf("\n");
  }
}

void getVerticalMovesDFS(struct Moves *moves, const struct Board *board,
                         const struct Piece *piece, const int limit) {
  if (piece == NULL) {
    TraceLog(LOG_DEBUG, "No piece on this position");
    return;
  }

  int directions[4][2] = {
      {-1, 1},  // left-up
      {1, 1},   // right-down
      {-1, -1}, // left-down
      {1, -1},  // right-up
  };

  for (int d = 0; d < 4; d++) {
    int x = piece->square.x;
    int y = piece->square.y;
    int distance = 0;
    while (x >= 0 && x < 8 && y >= 0 && y < 8 && distance < limit) {
      TraceLog(LOG_DEBUG, "Checking position (%d, %d)", x, y);
      distance++;

      x += directions[d][0];
      y += directions[d][1];

      if (x < 0 || x >= 8 || y < 0 || y >= 8) {
        TraceLog(LOG_DEBUG, "Position (%d, %d) is out of bounds", x, y);
        break;
      }

      if (piece->square.x == x && piece->square.y == y) {
        TraceLog(LOG_DEBUG, "The current location isn't a valid move");
        continue;
      }

      struct Piece *p = board->pieces[x][y];
      if (p != NULL) {
        if (p->player == piece->player) {
          TraceLog(LOG_DEBUG, "Reach current player piece on (%d, %d)", x, y);
          break;
        }
        TraceLog(LOG_DEBUG, "Reach other player piece on (%d, %d)", x, y);
        moves->squares[moves->size++] = p->square;
        break;
      }
      TraceLog(LOG_DEBUG, "ADDING MOVE(%d) TO THE LIST - CAPACITY",
               moves->size);
      moves->squares[moves->size] = (struct Vector2){.x = x, .y = y};
      moves->size++;
    }
  }
}

void getStraightMovesDFS(struct Moves *moves, const struct Board *board,
                         const struct Piece *piece, const int limit) {
  if (piece == NULL) {
    TraceLog(LOG_DEBUG, "No piece on this position");
    return;
  }

  int directions[4][2] = {
      {0, -1}, // up
      {1, 0},  // right
      {0, 1},  // down
      {-1, 0}, // left
  };

  for (int d = 0; d < 4; d++) {
    int x = piece->square.x;
    int y = piece->square.y;
    int distance = 0;
    while (x >= 0 && x < 8 && y >= 0 && y < 8 && distance < limit) {
      TraceLog(LOG_DEBUG, "Checking position (%d, %d)", x, y);
      distance++;

      x += directions[d][0];
      y += directions[d][1];

      // Boundary check
      if (x < 0 || x >= 8 || y < 0 || y >= 8) {
        TraceLog(LOG_DEBUG, "Position (%d, %d) is out of bounds", x, y);
        break;
      }

      if (piece->square.x == x && piece->square.y == y) {
        TraceLog(LOG_DEBUG, "The current location isn't a valid move");
        continue;
      }

      struct Piece *p = board->pieces[x][y];
      if (p != NULL) {
        if (p->player == piece->player) {
          TraceLog(LOG_DEBUG, "Reach current player piece on (%d, %d)", x, y);
          break;
        }
        TraceLog(LOG_DEBUG, "Reach other player piece on (%d, %d)", x, y);
        moves->squares[moves->size++] = p->square;
        break;
      }
      moves->squares[moves->size++] = (struct Vector2){.x = x, .y = y};
    }
  }
}

void getKnightPossibleMoves(struct Moves *moves, const struct Board *board,
                            const struct Piece *piece) {
  TraceLog(LOG_DEBUG, "Geting knight possibles move");
  static Vector2 possibleMoves[] = {
      (Vector2){.x = 2.0, .y = 1.0},  (Vector2){.x = 2.0, .y = -1.0},
      (Vector2){.x = -2.0, .y = 1.0}, (Vector2){.x = -2.0, .y = -1.0},
      (Vector2){.x = 1.0, .y = 2.0},  (Vector2){.x = 1.0, .y = -2.0},
      (Vector2){.x = -1.0, .y = 2.0}, (Vector2){.x = -1.0, .y = -2.0},
  };

  int x = piece->square.x;
  int y = piece->square.y;

  TraceLog(LOG_DEBUG, "Possible knight move from (%d, %d)", x, y);
  for (int i = 0; i < 8; i++) {
    int nextX = x + possibleMoves[i].x;
    int nextY = y + possibleMoves[i].y;

    if ((nextX < 0 || nextX >= 8) || (nextY < 0 || nextY >= 8))
      continue;

    struct Piece *pieceOnTheTargetSquare = board->pieces[nextX][nextY];
    if (pieceOnTheTargetSquare != NULL &&
        pieceOnTheTargetSquare->player == piece->player) {
      TraceLog(LOG_DEBUG, "Can't overwrite your own pieces");
      continue;
    }

    TraceLog(LOG_DEBUG, "\t %d - Possible move (%d, %d)", moves->size, nextX,
             nextY);
    moves->squares[moves->size] = (Vector2){.x = nextX, .y = nextY};
    moves->size++;
  }
}

void getPawnPossibleMoves(struct Moves *moves, const struct Board *board,
                          const struct Piece *piece) {
  int x = (int)piece->square.x;
  int y = (int)piece->square.y;
  int fowardMove = piece->player == WhitePlayer ? -1 : +1;

  // Double move
  int foward = y + fowardMove * 2;
  bool outOfBound = (foward < 0 || foward >= 8);
  if (!outOfBound && piece->moveCounter == 0 &&
      board->pieces[x][foward] == NULL) {
    moves->squares[moves->size++] = (Vector2){.x = x, .y = foward};
  }

  // Single move
  foward = y + fowardMove;
  outOfBound = (foward < 0 || foward >= 8);
  if (!outOfBound && board->pieces[x][foward] == NULL) {
    moves->squares[moves->size++] = (Vector2){.x = x, .y = foward};
  }

  // Capture moves (diagonal)
  int possibleMoves[2][2] = {
      {x + 1, foward},
      {x - 1, foward},
  };

  for (int i = 0; i < 2; i++) {
    int newX = possibleMoves[i][0];
    outOfBound = (foward < 0 || foward >= 8 || newX < 0 || newX >= 8);

    if (!outOfBound) {
      struct Piece *targetPiece = board->pieces[newX][foward];
      if (targetPiece != NULL && targetPiece->player != piece->player) {
        moves->squares[moves->size++] = (Vector2){.x = newX, .y = foward};
      }
    }
  }

  // TODO: En Passant
}

void GetPossibleMoves(struct Moves *moves, const struct Board *board,
                      const struct Piece *piece) {
  moves->size = 0;
  if (piece == NULL) {
    return;
  }

  switch (piece->type) {
  case Knight: {
    TraceLog(LOG_DEBUG, "Knight move");
    getKnightPossibleMoves(moves, board, piece);
    break;
  }
  case Bishop: {
    TraceLog(LOG_DEBUG, "Bishop move");
    getVerticalMovesDFS(moves, board, piece, 9999);
    break;
  }
  case Rook: {
    TraceLog(LOG_DEBUG, "Rook move");
    getStraightMovesDFS(moves, board, piece, 9999);
    break;
  }
  case Queen: {
    TraceLog(LOG_DEBUG, "Queen move");
    getStraightMovesDFS(moves, board, piece, 9999);
    getVerticalMovesDFS(moves, board, piece, 9999);
    break;
  }
  case King: {
    TraceLog(LOG_DEBUG, "King move");
    getStraightMovesDFS(moves, board, piece, 1);
    getVerticalMovesDFS(moves, board, piece, 1);
    // TODO: casting
    break;
  }
  case Pawn: {
    TraceLog(LOG_DEBUG, "Pawn move");
    getPawnPossibleMoves(moves, board, piece);
    break;
  }
  default:
    TraceLog(LOG_DEBUG, "This piece does not has implementation for this");
  }
}
