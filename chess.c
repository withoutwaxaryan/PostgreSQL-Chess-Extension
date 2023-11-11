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

#include "utils/builtins.h"
#include "libpq/pqformat.h"

#include "chess.h"

PG_MODULE_MAGIC;

/*****************************************************************************/

static Chess *
chess_make(double a)
{
  Chess *c = palloc0(sizeof(Chess));
  c->a = a;

  if (c->a == 0)
    c->a = 0;
  return c;
}

static Chess *
chess_parse(char *str)
{
  return chess_make(1);
}


/*****************************************************************************/
/* Just a dummy constructor to start */
PG_FUNCTION_INFO_V1(chess_constructor);
Datum
chess_constructor(PG_FUNCTION_ARGS)
{
  double a = PG_GETARG_FLOAT8(0);
  PG_RETURN_CHESS_P(chess_make(a));
}


/*****************************************************************************/
PG_FUNCTION_INFO_V1(chess_in);
Datum
chess_in(PG_FUNCTION_ARGS)
{
  char *str = PG_GETARG_CSTRING(0);
  return PointerGetDatum(chess_parse(str));
}

PG_FUNCTION_INFO_V1(chess_out);
Datum
chess_out(PG_FUNCTION_ARGS)
{
  
  char *result = "dummyresult";
  PG_RETURN_CSTRING(result);
}



