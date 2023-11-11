-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION chess" to load this file. \quit

/******************************************************************************
 * Input/Output
 ******************************************************************************/

CREATE OR REPLACE FUNCTION chess_in(cstring)
  RETURNS chess
  AS 'MODULE_PATHNAME'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION chess_out(chess)
  RETURNS cstring
  AS 'MODULE_PATHNAME'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;



CREATE TYPE chess (
  internallength = 8,
  input          = chess_in,
  output         = chess_out
);



/******************************************************************************
 * Constructor
 ******************************************************************************/

CREATE FUNCTION chess(double precision)
  RETURNS complex
  AS 'MODULE_PATHNAME', 'chess_constructor'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/*****************************************************************************
 * Accessing values
 *****************************************************************************/


/******************************************************************************
 * Operators
 ******************************************************************************/

