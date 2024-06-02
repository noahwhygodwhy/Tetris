#include "Piece.hpp"
#include "Board.hpp"

Piece::Piece(PieceType pt)
{
	this->rotation = 0;
	this->type = pt;
	uint32_t bbSize = pieceDictionary[uint32_t(pt)].BoundingBoxSize;
	uint32_t xLoc = bbSize % 2 == 0 ?
		((Board::width/2) - (bbSize / 2)) :
		((Board::width/2) - (bbSize / 2)) - 1;
	this->location = Coord2D(xLoc, 0);

	//this->location = bbSize %2 == 0? 
}

void Piece::Draw(HANDLE hStdout, Coord2D topleftOfBoard)
{
	PieceDescription pd = pieceDictionary[uint32_t(this->type)];

	SetConsoleTextAttribute(hStdout, uint32_t(this->type));
	for (uint32_t i = 0; i < 4; i++)
	{
		Coord2D abLoc = topleftOfBoard + this->location + pd.offsets[i];
		wprintf(L"\033[%d;%dH", abLoc.y, abLoc.x);
		wprintf(L"█");
	}
	SetConsoleTextAttribute(hStdout, 15);
}