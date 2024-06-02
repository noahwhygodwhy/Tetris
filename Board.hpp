#pragma once
#include <array>
#include "Piece.hpp"
using namespace std;



class Board
{


public:
	static const uint32_t height = 22;
	static const uint32_t width = 10;
	Board();
	~Board();
	array<array<Cell, height>, width> cells;

	// Tests if the piece will run into another piece (or the bottom)
	bool TestCollision(Piece* piece) const;

	// Tests if the peice is already overlapping with another piece (or the walls)
	bool TestOverlap(Piece* piece) const;
	
	// Filled in the board's cells for where the piece currently is
	void SolidifyPiece(Piece* piece);

private:
};