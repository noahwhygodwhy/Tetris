#pragma once
#include <array>
#include "Piece.hpp"
using namespace std;


class Board
{
public:
	Board();
	~Board();

private:
	array<array<Cell, 20>, 10> cells;
};