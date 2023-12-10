-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION chess" to load this file. \quit

/******************************************************************************
 * Input/Output chessboard
 ******************************************************************************/



CREATE OR REPLACE FUNCTION chessboard_in(cstring)
  RETURNS chessboard
  AS 'MODULE_PATHNAME'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION chessboard_out(chessboard)
  RETURNS cstring
  AS 'MODULE_PATHNAME'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE TYPE chessboard (
  internallength = 69,
  input          = chessboard_in,
  output         = chessboard_out
);

CREATE OR REPLACE FUNCTION chessboard(text)
  RETURNS chessboard
  AS 'MODULE_PATHNAME', 'chessboard_cast_from_text'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE CAST (text as chessboard) WITH FUNCTION chessboard(text) AS IMPLICIT;

/******************************************************************************
 * Input/Output chessgame
 ******************************************************************************/



CREATE OR REPLACE FUNCTION chessgame_in(cstring)
  RETURNS chessgame
  AS 'MODULE_PATHNAME'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION chessgame_out(chessgame)
  RETURNS cstring
  AS 'MODULE_PATHNAME'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE TYPE chessgame (
  internallength = 512,
  input          = chessgame_in,
  output         = chessgame_out
);

CREATE OR REPLACE FUNCTION chessgame(text)
  RETURNS chessgame
  AS 'MODULE_PATHNAME', 'chessgame_cast_from_text'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE CAST (text as chessgame) WITH FUNCTION chessgame(text) AS IMPLICIT;





/******************************************************************************
 * Constructors
 ******************************************************************************/

CREATE FUNCTION chessboard(double precision)
  RETURNS chessboard
  AS 'MODULE_PATHNAME', 'chessboard_constructor'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;


CREATE FUNCTION chessgame(double precision)
  RETURNS chessgame
  AS 'MODULE_PATHNAME', 'chessgame_constructor'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;


/******************************************************************************
 * Functions
 ******************************************************************************/

/*
getBoard(chessgame, integer) -> chessboard: Return the board state
at a given half-move (A full move is counted only when both players
have played). The integer parameter indicates the count of half
moves since the beginning of the game. A 0 value of this parameter
means the initial board state, i.e.,(
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1).
*/

CREATE FUNCTION getBoard(chessgame, integer)
  RETURNS chessboard
  AS 'MODULE_PATHNAME', 'getBoard'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;


/**
  getFirstMoves(chessgame, integer) -> chessgame: Returns the
  chessgame truncated to its first N half-moves. This function may also
  be called getOpening(...). Again the integer parameter is zero based.
*/
CREATE FUNCTION getFirstMoves(chessgame, integer)
  RETURNS chessgame
  AS 'MODULE_PATHNAME', 'getFirstMoves'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;


CREATE FUNCTION chessgame_compare(chessboard, chessboard)
    RETURNS int
    AS 'MODULE_PATHNAME', 'chessgame_compare'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_gin_extract_value(bigint, internal)
    RETURNS internal
    AS 'MODULE_PATHNAME', 'chessgame_gin_extract_value'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_gin_extract_query(bigint, internal, int2, internal, internal, internal, internal)
    RETURNS internal
    AS 'MODULE_PATHNAME', 'chessgame_gin_extract_query'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_gin_triconsistent(internal, int2, bigint, int4, internal, internal, internal)
    RETURNS char
    AS 'MODULE_PATHNAME', 'chessgame_gin_triconsistent'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
  
CREATE FUNCTION chessgame_contains_chessboard(chessgame, chessboard)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'chessgame_contains_chessboard'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR @> (
  PROCEDURE = chessgame_contains_chessboard,
  LEFTARG = chessgame, RIGHTARG = chessboard
);

CREATE FUNCTION hasBoard(cg chessgame, cb chessboard, i integer)
  RETURNS boolean
  AS 
$$
    SELECT cg @> cb and getFirstMoves(cg, i) @> cb;
$$
 LANGUAGE SQL;

  -- Create the operator class
CREATE OPERATOR CLASS chessboard_gin_ops
    DEFAULT FOR TYPE ChessGame USING gin AS
    OPERATOR   7 @> (chessgame, chessboard),
    FUNCTION   1    chessgame_compare(chessboard, chessboard),
    FUNCTION   2    chessgame_gin_extract_value(bigint, internal),
    FUNCTION   3    chessgame_gin_extract_query(bigint, internal, int2, internal, internal, internal, internal),
    FUNCTION   4    chessgame_gin_triconsistent(internal, int2, bigint, int4, internal, internal, internal);


/*****************************************************************************/
CREATE FUNCTION hasOpening(game1 chessgame, game2 chessgame)
  RETURNS boolean
  AS $$
    select hasOpening_cmp(game1, game2) = 0; 
  $$
  LANGUAGE SQL;

/******************************************************************************/

/* B-Tree comparison functions */

CREATE OR REPLACE FUNCTION hasOpening_eq(chessgame, chessgame)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'hasOpening_eq'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION hasOpening_lt(chessgame, chessgame)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'hasOpening_lt'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION hasOpening_le(chessgame, chessgame)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'hasOpening_le'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION hasOpening_gt(chessgame, chessgame)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'hasOpening_gt'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION hasOpening_ge(chessgame, chessgame)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'hasOpening_ge'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;


/******************************************************************************/

/* B-Tree comparison operators */

CREATE OPERATOR = (
  LEFTARG = chessgame, RIGHTARG = chessgame,
  PROCEDURE = hasOpening_eq
  -- COMMUTATOR = =, NEGATOR = <>
);
CREATE OPERATOR < (
  LEFTARG = chessgame, RIGHTARG = chessgame,
  PROCEDURE = hasOpening_lt
  -- COMMUTATOR = >, NEGATOR = >=
);
CREATE OPERATOR <= (
  LEFTARG = chessgame, RIGHTARG = chessgame,
  PROCEDURE = hasOpening_le
  -- COMMUTATOR = >=, NEGATOR = >
);
CREATE OPERATOR >= (
  LEFTARG = chessgame, RIGHTARG = chessgame,
  PROCEDURE = hasOpening_ge
  -- COMMUTATOR = <=, NEGATOR = <
);
CREATE OPERATOR > (
  LEFTARG = chessgame, RIGHTARG = chessgame,
  PROCEDURE = hasOpening_gt
  -- COMMUTATOR = <, NEGATOR = <=
);

/******************************************************************************/

/* B-Tree support function */

CREATE OR REPLACE FUNCTION hasOpening_cmp(chessgame, chessgame)
  RETURNS integer
  AS 'MODULE_PATHNAME', 'hasOpening_cmp'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/******************************************************************************/

/* B-Tree operator class */

CREATE OPERATOR CLASS chessgame_hasopening_ops
DEFAULT FOR TYPE chessgame USING btree
AS
        OPERATOR        1       <  ,
        OPERATOR        2       <= ,
        OPERATOR        3       =  ,
        OPERATOR        4       >= ,
        OPERATOR        5       >  ,
        FUNCTION        1       hasOpening_cmp(chessgame, chessgame);

/******************************************************************************/