#pragma once

#include <array>

using namespace std;

enum class Cell
{
	EMPTY,
	LIGHT_BLUE,
	DARK_BLUE,
	ORANGE,
	YELLOW,
	GREEN,
	RED,
	MAGENTA,
	COUNT,
};

enum class PieceType : uint32_t
{
	I,
	O,
	T,
	S,
	Z,
	J,
	L,
	COUNT
};
 

// all coords are assuming the vertex top left is (0, 0)
// and if the coord is used to describe a square's location
// then th square extends down right
struct Coord2D
{
	uint32_t x;
	uint32_t y;
	Coord2D(uint32_t x, uint32_t y) : x(x), y(y) {}
};
struct PieceDescription
{
	uint32_t BoundingBoxSize;
	array<Coord2D, 4> offsets; // all pieces have four squares
	Cell color;
};


//All of this is based on SRS, idk how ARS works
static array<PieceDescription, uint32_t(PieceType::COUNT)> pieceDictionary = {
/*I*/ PieceDescription{4, {Coord2D(0, 1), Coord2D(1, 1), Coord2D(2, 1), Coord2D(3, 1)}, Cell::LIGHT_BLUE},
/*O*/ PieceDescription{2, {Coord2D(0, 0), Coord2D(0, 1), Coord2D(1, 0), Coord2D(1, 1)}, Cell::YELLOW},
/*T*/ PieceDescription{3, {Coord2D(1, 0), Coord2D(0, 1), Coord2D(1, 1), Coord2D(2, 1)}, Cell::MAGENTA},
/*S*/ PieceDescription{3, {Coord2D(1, 0), Coord2D(2, 0), Coord2D(0, 1), Coord2D(1, 1)}, Cell::GREEN},
/*Z*/ PieceDescription{3, {Coord2D(0, 0), Coord2D(1, 0), Coord2D(1, 1), Coord2D(2, 1)}, Cell::RED},
/*J*/ PieceDescription{3, {Coord2D(0, 0), Coord2D(0, 1), Coord2D(1, 1), Coord2D(2, 1)}, Cell::DARK_BLUE},
/*L*/ PieceDescription{3, {Coord2D(2, 1), Coord2D(0, 1), Coord2D(1, 1), Coord2D(2, 1)}, Cell::ORANGE}
};

class Piece
{
	Coord2D location; //top left of the pieces bounding box (not strictly a "filled" square)
	PieceType type;
	uint32_t rotation = 0; //0 to 3, what this actually means tbd
};