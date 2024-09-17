#ifndef GAME_H
#define GAME_H

#define PIECE_IMG_SIZE 75
#define SQUARE_SIZE 80
#define BOARD_SIZE 8

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
};

struct Board {
  unsigned int size;
  struct Piece ***pieces;
};

struct Moves {
  Vector2 squares;
  unsigned size;
};

static Texture2D _whitePieceTextures[6];
static Texture2D _blackPieceTextures[6];
static struct Piece _whitePieces[16];
static struct Piece _blackPieces[16];
static struct Board _board;

void Init();
void Deinit();
void Reset();
struct Piece *GetPieceInXYPosition(unsigned x, unsigned y);
Vector2 GetSquareOverlabByTheCursor();
struct Moves GetPossibleMoves(struct Piece *piece);
Texture2D *getPieceTexture(const struct Piece *piece);

#endif // GAME_H
