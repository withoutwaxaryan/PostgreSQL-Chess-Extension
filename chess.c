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

PG_FUNCTION_INFO_V1(getBoard);
Datum
getBoard(PG_FUNCTION_ARGS)
{
  ChessGame *cg = PG_GETARG_CHESSGAME_P(0);
  int halfMove = PG_GETARG_INT32(1);

  
  ChessBoard *cb = palloc0(sizeof(ChessBoard));

  SCL_recordApply(cg->record, cb ,halfMove);

  PG_FREE_IF_COPY(cg, 0);

  PG_RETURN_CSTRING(cb);
}

