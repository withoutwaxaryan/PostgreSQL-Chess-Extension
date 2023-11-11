/*
 * chess.h
 */

/******************************STRUCTURES***************************************/


typedef struct
{

  char* fen;

} ChessBoard;


/* fmgr macros chessboard type */

  #define ChessBoardPGetDatum(X)  PointerGetDatum(X)
  
  #define PG_RETURN_CHESSBOARD_P(x) return ChessBoardPGetDatum(x)


  #define DatumGetChessBoardP(X)  ((ChessBoard *) DatumGetPointer(X))
 
  #define PG_GETARG_CHESSBOARD_P(n) DatumGetChessBoardP(PG_GETARG_DATUM(n))
/*****************************************************************************/


