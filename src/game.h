#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
  Texture2D *img;
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
static struct Piece _whitePieces[6];
static struct Piece _blackPieces[6];
static struct Board _board;

void Init();
void Deinit();
void Reset();
struct Piece *GetPieceInXYPosition(unsigned x, unsigned y);

#endif // GAME_H
