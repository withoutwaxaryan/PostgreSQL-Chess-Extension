/*
 * chess.h
 */

/* Dummy chess structure */
typedef struct
{
  double    a;
} Chess;

/* fmgr macros chess type */


 #define ChessPGetDatum(X)  PointerGetDatum(X)

 #define PG_RETURN_CHESS_P(x) return ChessPGetDatum(x)

/*****************************************************************************/


