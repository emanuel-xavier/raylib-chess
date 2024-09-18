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

struct Game {
  struct Board *board;
  struct Piece _whitePieces[16];
  struct Piece _blackPieces[16];
  enum Player _currentPlayer;
};

static Texture2D _whitePieceTextures[6];
static Texture2D _blackPieceTextures[6];

struct Game *NewGame();
void DeleteGame(struct Game *game);
void ResetDefaultConfiguration(struct Game *game);
struct Piece *GetPieceInXYPosition(const struct Game *game, unsigned x,
                                   unsigned y);
Texture2D *GetPieceTexture(const struct Piece *piece);
void MovePiece(struct Game *game, struct Piece *piece, const Vector2 *pos);
struct Moves GetPossibleMoves(const struct Game *game, struct Piece *piece);
enum Player GetCurrentPlayer(const struct Game *game);
enum Player NextPlayer(struct Game *Game);

Vector2 GetSquareOverlabByTheCursor();

#endif // GAME_H
