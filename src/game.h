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
static enum Player _currentPlayer = WhitePlayer;

void Init();
void Deinit();
void Reset();
struct Piece *GetPieceInXYPosition(unsigned x, unsigned y);
Vector2 GetSquareOverlabByTheCursor();
struct Moves GetPossibleMoves(struct Piece *piece);
Texture2D *getPieceTexture(const struct Piece *piece);
enum Player GetCurrentPlayer();
enum Player NextPlayer();

inline static void _debug_printBoard() {
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      if (_board.pieces[x][y] != NULL) {
        printf(" %d-%d ", (int)_board.pieces[x][y]->square.x,
               (int)_board.pieces[x][y]->square.y);
      } else {
        printf("       ");
      }
    }
    printf("\n");
  }
}

#endif // GAME_H
