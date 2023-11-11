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

#include "utils/builtins.h"
#include "libpq/pqformat.h"

#include "chess.h"

PG_MODULE_MAGIC;

/*****************************************************************************/


// create a chessboard datatype with a constructor takes FEN notation as input
static ChessBoard *
chessboard_make(char *fen)
{
  ChessBoard *cb = palloc0(sizeof(ChessBoard));
  cb->fen = strdup(fen);
  return cb;
}


static ChessBoard *
chessboard_parse(char *fen)
{
  return chessboard_make(fen);
}

static char *
chessboard_to_str(const ChessBoard *cb)
{
  char *result = psprintf("%s", cb->fen);
  return result;
}


/*****************************************************************************/

PG_FUNCTION_INFO_V1(chessboard_constructor);
Datum
chessboard_constructor(PG_FUNCTION_ARGS)
{
  char *fen = PG_GETARG_CSTRING(0);
  PG_RETURN_CHESSBOARD_P(chessboard_parse(fen));
}


/*********************************INPUT OUTPUT*****************************************/


/* in out cast functions of CHESSBOARD*/

PG_FUNCTION_INFO_V1(chessboard_in);
Datum
chessboard_in(PG_FUNCTION_ARGS)
{
  char *str = PG_GETARG_CSTRING(0);
  return PointerGetDatum(chessboard_parse(str));
}

PG_FUNCTION_INFO_V1(chessboard_out);
Datum
chessboard_out(PG_FUNCTION_ARGS)
{
 

  ChessBoard *cb = PG_GETARG_CHESSBOARD_P(0);
  char *result = chessboard_to_str(cb);
  PG_FREE_IF_COPY(cb, 0);

  PG_RETURN_CSTRING(result);
}


PG_FUNCTION_INFO_V1(chessboard_cast_from_text);
Datum
chessboard_cast_from_text(PG_FUNCTION_ARGS)
{
  text *txt = PG_GETARG_TEXT_P(0);
  char *str = DatumGetCString(DirectFunctionCall1(textout,
               PointerGetDatum(txt)));
  PG_RETURN_CHESSBOARD_P(chessboard_parse(str));
}


