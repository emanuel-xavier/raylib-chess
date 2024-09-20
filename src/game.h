#ifndef GAME_H
#define GAME_H

#define PIECE_IMG_SIZE 75
#define SQUARE_SIZE 80
#define BOARD_SIZE 8

#define DEBUG_MODE
// #define PRINT_BOARD

#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"

enum PieceType {
  Pawn = 0,
  Knight,
  Bishop,
  King,
  Rook,
  Queen,
};

enum Player {
  WhitePlayer = 0,
  BlackPlayer,
};

struct Piece {
  enum Player player;
  enum PieceType type;
  Vector2 square;
  Vector2 pos;
  unsigned moveCounter;
};

struct Board {
  unsigned int size;
  struct Piece ***pieces;
};

struct Moves {
  Vector2 squares[21];
  unsigned size;
};

struct Game {
  struct Board *board;
  struct Piece _whitePieces[16];
  struct Piece _blackPieces[16];
  enum Player _currentPlayer;
  struct Moves moves;
};

struct Game *NewGame();
void DeleteGame(struct Game *game);
void ResetDefaultConfiguration(struct Game *game);
struct Piece *GetPieceInXYPosition(const struct Game *game, unsigned x,
                                   unsigned y);
bool MovePiece(struct Game *game, struct Piece *piece, const Vector2 *pos);
enum Player GetCurrentPlayer(const struct Game *game);
enum Player NextPlayer(struct Game *Game);
void GetPossibleMoves(struct Moves *moves, const struct Board *board,
                      const struct Piece *piece);

#define WHITE_PLAYER 'W'
#define BLACK_PLAYER 'B'

#define PAWN 'P'
#define ROOK 'R'
#define KNIGHT 'N'
#define BISHOP 'B'
#define QUEEN 'Q'
#define KING 'K'

// Function to get character representation for a piece
char getPieceChar(const struct Piece *piece);
void PrintFormattedBoard(const struct Game *game);

#endif // GAME_H
