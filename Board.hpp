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

	bool TestCollision(Piece* piece);
	void SolidifyPiece(Piece* piece);

private:
};