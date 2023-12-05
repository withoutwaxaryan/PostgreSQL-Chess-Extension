/*
 * chess.C
 *
 * PostgreSQL chess type:
 *
 * chess '(a)'
 *
 * Author: Berat Furkan Kocak (berat.kocak@ulb.be), David ... , Celia ... , Aryan ...
 */
#include <stdio.h>
#include <postgres.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <catalog/pg_type.h>
#include <access/gin.h>
#include <access/stratnum.h>
#include <utils/builtins.h>
#include <libpq/pqformat.h>

#include "smallchesslib.h"
#include "chess.h"

PG_MODULE_MAGIC;

/*****************************************************************************/

// create a chessboard datatype with a constructor takes FEN notation as input
static ChessBoard *
chessboard_make(SCL_Board board, char *fen)
{
  ChessBoard *cb = palloc0(sizeof(ChessBoard));

  // Copy the array elements
  SCL_boardCopy(board, cb->board);

  return cb;
}

static ChessBoard *
chessboard_parse(char *fen)
{
  SCL_Board startState = SCL_BOARD_START_STATE;
  SCL_boardFromFEN(startState, fen);
  return chessboard_make(startState, fen);
}

static char *
chessboard_to_str(const ChessBoard *cb)
{
  // char *result = psprintf("%s", cb->fen);

  char *result = palloc0(sizeof(char) * SCL_FEN_MAX_LENGTH);
  SCL_boardToFEN(cb->board, result);
  return result;
}

/*****************************************************************************/

PG_FUNCTION_INFO_V1(chessboard_constructor);
Datum chessboard_constructor(PG_FUNCTION_ARGS)
{
  char *fen = PG_GETARG_CSTRING(0);
  PG_RETURN_CHESSBOARD_P(chessboard_parse(fen));
}

/*********************************INPUT OUTPUT*****************************************/

/* in out cast functions of CHESSBOARD*/

PG_FUNCTION_INFO_V1(chessboard_in);
Datum chessboard_in(PG_FUNCTION_ARGS)
{
  char *str = PG_GETARG_CSTRING(0);
  return PointerGetDatum(chessboard_parse(str));
}

PG_FUNCTION_INFO_V1(chessboard_out);
Datum chessboard_out(PG_FUNCTION_ARGS)
{

  ChessBoard *cb = PG_GETARG_CHESSBOARD_P(0);
  char *result = chessboard_to_str(cb);
  PG_FREE_IF_COPY(cb, 0);

  PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(chessboard_cast_from_text);
Datum chessboard_cast_from_text(PG_FUNCTION_ARGS)
{
  text *txt = PG_GETARG_TEXT_P(0);
  char *str = DatumGetCString(DirectFunctionCall1(textout,
                                                  PointerGetDatum(txt)));
  PG_RETURN_CHESSBOARD_P(chessboard_parse(str));
}

/*****************************************************************************/

// create a chessboard datatype with a constructor takes FEN notation as input
static ChessGame *
chessgame_make(SCL_Record record, char *pgn)
{
  ChessGame *cg = palloc0(sizeof(ChessGame));

  // Copy the array elements
  SCL_recordCopy(record, cg->record);
  return cg;
}

static ChessGame *
chessgame_parse(char *pgn)
{
  SCL_Record record;
  SCL_recordFromPGN(record, pgn);
  return chessgame_make(record, pgn);
}

static char *
chessgame_to_str(const ChessGame *cg)
{
  char *result = palloc0(sizeof(char) * SCL_RECORD_MAX_SIZE);
  SCL_printPGN(cg->record, result, 0);
  return result;
}

/*****************************************************************************/

PG_FUNCTION_INFO_V1(chessgame_constructor);
Datum chessgame_constructor(PG_FUNCTION_ARGS)
{
  char *pgn = PG_GETARG_CSTRING(0);
  PG_RETURN_CHESSGAME_P(chessgame_parse(pgn));
}

/*********************************INPUT OUTPUT*****************************************/

/* in out cast functions of chessgame */

PG_FUNCTION_INFO_V1(chessgame_in);
Datum chessgame_in(PG_FUNCTION_ARGS)
{
  char *str = PG_GETARG_CSTRING(0);
  return PointerGetDatum(chessgame_parse(str));
}

PG_FUNCTION_INFO_V1(chessgame_out);
Datum chessgame_out(PG_FUNCTION_ARGS)
{
  ChessGame *cg = PG_GETARG_CHESSGAME_P(0);
  char *result = chessgame_to_str(cg);
  PG_FREE_IF_COPY(cg, 0);

  PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(chessgame_cast_from_text);
Datum chessgame_cast_from_text(PG_FUNCTION_ARGS)
{
  text *txt = PG_GETARG_TEXT_P(0);
  char *str = DatumGetCString(DirectFunctionCall1(textout,
                                                  PointerGetDatum(txt)));
  PG_RETURN_CHESSGAME_P(chessgame_parse(str));
}

/*********************************Functions*****************************/
/*
getBoard(chessgame, integer) -> chessboard: Return the board state
at a given half-move (A full move is counted only when both players
have played). The integer parameter indicates the count of half
moves since the beginning of the game. A 0 value of this parameter
means the initial board state, i.e.,(
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1).
*/

PG_FUNCTION_INFO_V1(getBoard);
Datum getBoard(PG_FUNCTION_ARGS)
{
  ChessGame *cg = PG_GETARG_CHESSGAME_P(0);
  int halfMove = PG_GETARG_INT32(1);

  ChessBoard *cb = palloc0(sizeof(ChessBoard));

  SCL_recordApply(cg->record, cb, halfMove);

  PG_FREE_IF_COPY(cg, 0);

  PG_RETURN_CHESSBOARD_P(cb);
}

/**
  getFirstMoves(chessgame, integer) -> chessgame: Returns the
  chessgame truncated to its first N half-moves. This function may also
  be called getOpening(...). Again the integer parameter is zero based.
*/

PG_FUNCTION_INFO_V1(getFirstMoves);
Datum getFirstMoves(PG_FUNCTION_ARGS)
{
  ChessGame *originalGame = PG_GETARG_CHESSGAME_P(0);
  int nOfHalfMoves = PG_GETARG_INT32(1);

  ChessGame *cg = palloc0(sizeof(ChessGame));
  SCL_recordCopy(originalGame->record, cg->record);

  int shouldContinue = 1;
  uint16_t length = SCL_recordLength(cg->record);
  for (uint16_t i = 0; i < (length - nOfHalfMoves) && shouldContinue; i++)
  {
    shouldContinue = SCL_recordRemoveLast(cg->record);
  }
  PG_FREE_IF_COPY(originalGame, 0);

  PG_RETURN_CHESSGAME_P(cg);
}

bool chessgameContainsChessboard(ChessGame *cg, ChessBoard *cb, int halfMoves)
{
  ChessBoard *temp = palloc0(sizeof(ChessBoard));

  for (uint16_t i = 0; i <= halfMoves; i++)
  {
    SCL_recordApply(cg->record, temp->board, i);

    uint32_t hash = SCL_boardHash32(temp->board);
    if (hash == SCL_boardHash32(cb->board))
    {
      return true;
    }
  }
  return false;
}

/*
hasBoard(chessgame, chessboard, integer) -> bool: Returns true if
the chessgame contains the given board state in its first N
half-moves. Only compare the state of the pieces and not compare
the move count, castling right, en passant pieces, ...

*/

PG_FUNCTION_INFO_V1(hasBoard);
Datum hasBoard(PG_FUNCTION_ARGS)
{
  ChessGame *cg = PG_GETARG_CHESSGAME_P(0);
  ChessBoard *cb = PG_GETARG_CHESSBOARD_P(1);
  int halfMove = PG_GETARG_INT32(2);

  bool result = chessgameContainsChessboard(cg, cb, halfMove);
  PG_FREE_IF_COPY(cg, 0);
  PG_FREE_IF_COPY(cb, 1);

  PG_RETURN_BOOL(result);
}

bool chessgame_contains_chessgame(ChessGame *c1, ChessGame *c2)
{
  // Get number of half moves of the 1st chess game
  uint16_t length1 = SCL_recordLength(c1->record);

  // Get number of half moves of the 2nd chess game
  uint16_t length2 = SCL_recordLength(c2->record);

  // Determine minimum length betwenn these two chess games
  uint8_t minLength = (length1 < length2) ? length1 : length2;

  // Check if the chessgame matches with minLength
  for (uint16_t i = 0; i < minLength; i++)
  {
    uint8_t squareFrom1;
    uint8_t squareTo1;
    uint8_t squareFrom2;
    uint8_t squareTo2;
    char promotedPiece;
    uint8_t mov1 = SCL_recordGetMove(c1->record, i, &squareFrom1, &squareTo1, &promotedPiece);
    uint8_t mov2 = SCL_recordGetMove(c2->record, i, &squareFrom2, &squareTo2, &promotedPiece);

    if (squareFrom1 != squareFrom2 || squareTo1 != squareTo2)
    {
      return false;
    }
  }
  return true;
}

/*
hasOpening(chessgame, chessgame) -> bool: Returns true if the first
chess game starts with the exact same set of moves as the second
chess game. The second parameter should not be a full game, but
should only contain the opening moves that we want to check for,
which can be of any length, i.e., m half-moves.
*/

PG_FUNCTION_INFO_V1(hasOpening);
Datum hasOpening(PG_FUNCTION_ARGS)
{
  // Get chessgames from arguments
  ChessGame *chessgame1 = PG_GETARG_CHESSGAME_P(0);
  ChessGame *chessgame2 = PG_GETARG_CHESSGAME_P(1);

  bool result = chessgame_contains_chessgame(chessgame1, chessgame2);
  PG_FREE_IF_COPY(chessgame1, 0);
  PG_FREE_IF_COPY(chessgame2, 1);
  PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(chessgame_gin_extract_value);
Datum chessgame_gin_extract_value(PG_FUNCTION_ARGS)
{
  ChessGame *cg = PG_GETARG_CHESSGAME_P(0);
  int32 *nkeys = (int32 *)PG_GETARG_POINTER(1);
  bool **nullFlags = (bool **)PG_GETARG_POINTER(2);
  Datum *entries = NULL;
  *nullFlags = NULL; // Assume all keys are non-null

  if (PG_ARGISNULL(0))
  {
    *nkeys = 0;
    PG_RETURN_NULL();
  }

  uint16_t length = SCL_recordLength(cg->record);
  if (length > 0)
  {
    entries = (Datum *)palloc(sizeof(Datum) * length + 1);
    *nkeys = length + 1;
  }
  for (uint16_t i = 0; i <= length; ++i)
  {
    ChessBoard *cb = palloc0(sizeof(ChessBoard));
    SCL_recordApply(cg->record, cb->board, i);
    entries[i] = ChessBoardPGetDatum(cb);
  }

  PG_RETURN_POINTER(entries);
}

PG_FUNCTION_INFO_V1(chessgame_gin_extract_query);
Datum chessgame_gin_extract_query(PG_FUNCTION_ARGS)
{
  Datum query = PG_GETARG_DATUM(0);
  int32 *nkeys = (int32 *)PG_GETARG_POINTER(1);
  StrategyNumber strategy = PG_GETARG_UINT16(2);
  bool **pmatch = (bool **)PG_GETARG_POINTER(3);
  Pointer **extra_data = (Pointer **)PG_GETARG_POINTER(4);
  bool **nullFlags = (bool **)PG_GETARG_POINTER(5);
  int32 *searchMode = (int32 *)PG_GETARG_POINTER(6);

  *nullFlags = NULL; // Assume all keys are non-null
  *searchMode = GIN_SEARCH_MODE_DEFAULT;

  if (PG_ARGISNULL(0))
  {
    *nkeys = 0;
    PG_RETURN_NULL();
  }

  switch (strategy)
  {
  case RTContainsStrategyNumber:
  {
    *nkeys = 1;
    Datum *states = (Datum *)palloc(sizeof(Datum));
    states[0] = query;
    // For simplicity, assume all keys require exact match (no partial match)
    *pmatch = (bool *)palloc(sizeof(bool));
    **pmatch = false;

    // Assume no extra data is needed for this example
    *extra_data = NULL;

    // Assume no null flags for this example

    // Assume default search mode
    *searchMode = GIN_SEARCH_MODE_DEFAULT;

    PG_RETURN_POINTER(states);
  }
  break;
  default:
    elog(ERROR, "chessgame_gin_extract_query: unknown strategy number: %d", strategy);
  }

  PG_RETURN_NULL();
}

PG_FUNCTION_INFO_V1(chessgame_compare);
Datum chessgame_compare(PG_FUNCTION_ARGS)
{
  ChessGame *a = PG_GETARG_CHESSGAME_P(0);
  ChessGame *b = PG_GETARG_CHESSGAME_P(1);

  int result = DatumGetInt32(SCL_recordLength(a->record) - SCL_recordLength(b->record));

  PG_FREE_IF_COPY(a, 0);
  PG_FREE_IF_COPY(b, 1);
  PG_RETURN_INT32(result);
}

PG_FUNCTION_INFO_V1(chessgame_gin_triconsistent);
Datum chessgame_gin_triconsistent(PG_FUNCTION_ARGS)
{
  GinTernaryValue *check = (GinTernaryValue *)PG_GETARG_POINTER(0);
  StrategyNumber strategy = PG_GETARG_UINT16(1);
  int32 nkeys = PG_GETARG_INT32(3);
  bool *nullFlags = (bool *)PG_GETARG_POINTER(6);
  GinTernaryValue res;
  int32 i;

  PG_RETURN_GIN_TERNARY_VALUE(GIN_TRUE);
}

PG_FUNCTION_INFO_V1(chessgame_contains_chessboard);
Datum chessgame_contains_chessboard(PG_FUNCTION_ARGS)
{
  ChessGame *cg = PG_GETARG_CHESSGAME_P(0);
  ChessBoard *cb = PG_GETARG_CHESSBOARD_P(1);

  bool result = chessgameContainsChessboard(cg, cb, SCL_recordLength(cg->record));
  PG_FREE_IF_COPY(cg, 0);
  PG_FREE_IF_COPY(cb, 1);

  PG_RETURN_BOOL(result);
}