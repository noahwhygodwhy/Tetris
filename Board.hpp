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
	//for these two, height's 0 index is the top, and height's <height> index is the bottom (apparently this is wrong
	array<array<Cell, height>, width> cells;
	array<bool, height> empty;
	
	// Tests if the piece will run into another piece (or the bottom)
	bool TestCollision(Piece* piece) const;

	// Tests if the peice is already overlapping with another piece (or the walls)
	bool TestOverlap(Piece* piece) const;
	
	// Filled in the board's cells for where the piece currently is
	void SolidifyPiece(Piece* piece);

	uint32_t tick();

private:
};