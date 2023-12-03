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
#include "smallchesslib.h"

#include "utils/builtins.h"
#include "libpq/pqformat.h"

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
  // return PointerGetDatum(chessboard_parse(str));
  PG_RETURN_CHESSBOARD_P(chessboard_parse(str));
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
  // PG_RETURN_CHESSBOARD_P(chessboard_parse(str));
  PG_RETURN_CHESSBOARD_P(chessboard_parse(str));
}

/*****************************************************************************/

// create a chessgame datatype with a constructor takes SAN notation as input
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
  // PG_RETURN_CHESSGAME_P(chessgame_parse(pgn));
  PG_RETURN_CHESSGAME_P(chessgame_parse(pgn));
}

/*********************************INPUT OUTPUT*****************************************/

/* in out cast functions of chessgame */

PG_FUNCTION_INFO_V1(chessgame_in);
Datum chessgame_in(PG_FUNCTION_ARGS)
{
  char *str = PG_GETARG_CSTRING(0);
  // return PointerGetDatum(chessgame_parse(str));
  PG_RETURN_CHESSGAME_P(chessgame_parse(str));
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
  // PG_RETURN_CHESSGAME_P(chessgame_parse(str));
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

  ChessBoard *temp = palloc0(sizeof(ChessBoard));

  for (int i = 0; i <= halfMove; i++)
  {

    //I couldnt call getboard because compiler complained about function args.

    SCL_recordApply(cg->record, temp, i); // maybe not correct way of doing it.
    
    char *str = chessboard_to_str(temp);
    if (strcmp(str, chessboard_to_str(cb)) == 0) // not the whole board but first part should be checked.
    {
      PG_FREE_IF_COPY(cg, 0);
      PG_FREE_IF_COPY(cb, 1);
      PG_RETURN_BOOL(true);
    }
  }


  PG_FREE_IF_COPY(cg, 0);
  PG_FREE_IF_COPY(cb, 1);

  PG_RETURN_BOOL(false);
}


//INTERNAL FUNCTION
static int hasOpening_cmp_internal(ChessGame *chessgame1, ChessGame *chessgame2);
// Implementation of the internal function
static int hasOpening_cmp_internal(ChessGame *chessgame1, ChessGame *chessgame2)
{
    int result;
    int value;

    // new chessgame variable
    ChessGame *cg1 = palloc0(sizeof(ChessGame));
    SCL_recordCopy(chessgame1->record, cg1->record);

    // new chessgame variable
    ChessGame *cg2 = palloc0(sizeof(ChessGame));
    SCL_recordCopy(chessgame2->record, cg2->record);

    // Compare the string representations of the two chess games
    value = strcmp(chessgame_to_str(cg1), chessgame_to_str(cg2));

    if (value < 0)
    {
       result = -1;
    }
    else if (value == 0)
    {
       result = 0;
    }
    else if (value > 0)
    {
      result = 1;
    }

    pfree(cg1);
    pfree(cg2);
    
    return result;
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

  // new chessgame variable
  ChessGame *cg1 = palloc0(sizeof(ChessGame));
  SCL_recordCopy(chessgame1->record, cg1->record);

  // new chessgame variable
  ChessGame *cg2 = palloc0(sizeof(ChessGame));
  SCL_recordCopy(chessgame2->record, cg2->record);

  // Get number of half moves of the 1st chess game
  uint16_t length1 = SCL_recordLength(cg1->record);

  // Get number of half moves of the 2nd chess game
  uint16_t length2 = SCL_recordLength(cg2->record);

  // Get the string of 1st chess game truncated to opening number of half moves
  // of the 2nd chess game
  int shouldContinue = 1;
  for (uint16_t i = 0; i < (length1 - length2) && shouldContinue; i++)
  {
    shouldContinue = SCL_recordRemoveLast(cg1->record);
  }

  // Compare the string representations of the two chess games
  bool value = (strcmp(chessgame_to_str(cg1), chessgame_to_str(cg2)) == 0);

  pfree(cg1);
  pfree(cg2);

  PG_FREE_IF_COPY(chessgame1, 0);
  PG_FREE_IF_COPY(chessgame2, 1);

  PG_RETURN_BOOL(value);

}

 /******************************************************************************
 * B-Tree related functions
 ******************************************************************************/

PG_FUNCTION_INFO_V1(hasOpening_lt);
Datum hasOpening_lt(PG_FUNCTION_ARGS)
{
	ChessGame *chessgame1 = PG_GETARG_CHESSGAME_P(0);
  ChessGame *chessgame2 = PG_GETARG_CHESSGAME_P(1);

	PG_RETURN_BOOL(hasOpening_cmp_internal(chessgame1, chessgame2) < 0);
}

PG_FUNCTION_INFO_V1(hasOpening_le);
Datum hasOpening_le(PG_FUNCTION_ARGS)
{
	ChessGame *chessgame1 = PG_GETARG_CHESSGAME_P(0);
  ChessGame *chessgame2 = PG_GETARG_CHESSGAME_P(1);

	PG_RETURN_BOOL(hasOpening_cmp_internal(chessgame1, chessgame2) <= 0);
}

PG_FUNCTION_INFO_V1(hasOpening_eq);
Datum hasOpening_eq(PG_FUNCTION_ARGS)
{
	ChessGame *chessgame1 = PG_GETARG_CHESSGAME_P(0);
  ChessGame *chessgame2 = PG_GETARG_CHESSGAME_P(1);

	PG_RETURN_BOOL(hasOpening_cmp_internal(chessgame1, chessgame2) == 0);
}

PG_FUNCTION_INFO_V1(hasOpening_ge);
Datum hasOpening_ge(PG_FUNCTION_ARGS)
{
	ChessGame *chessgame1 = PG_GETARG_CHESSGAME_P(0);
  ChessGame *chessgame2 = PG_GETARG_CHESSGAME_P(1);

	PG_RETURN_BOOL(hasOpening_cmp_internal(chessgame1, chessgame2) >= 0);
}

PG_FUNCTION_INFO_V1(hasOpening_gt);
Datum hasOpening_gt(PG_FUNCTION_ARGS)
{
	ChessGame *chessgame1 = PG_GETARG_CHESSGAME_P(0);
  ChessGame *chessgame2 = PG_GETARG_CHESSGAME_P(1);

	PG_RETURN_BOOL(hasOpening_cmp_internal(chessgame1, chessgame2) > 0);
}


PG_FUNCTION_INFO_V1(chessgame_hasOpening_cmp);
Datum chessgame_hasOpening_cmp(PG_FUNCTION_ARGS)
{
    int result;
    ChessGame *chessgame1 = PG_GETARG_CHESSGAME_P(0);
    ChessGame *chessgame2 = PG_GETARG_CHESSGAME_P(1);

    result = hasOpening_cmp_internal(chessgame1, chessgame2);

    PG_FREE_IF_COPY(chessgame1, 0);
    PG_FREE_IF_COPY(chessgame2, 1);

    PG_RETURN_INT32(result);
}
