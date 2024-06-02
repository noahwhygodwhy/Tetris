// score
// 10x20 cells
// next piece preview
// hold piece location
#include "Board.hpp"

Board::Board()
{
	this->cells = array<array<Cell, height>, width>();
	for (uint32_t i = 0; i < width; i++)
	{
		this->cells[i] = array<Cell, height>();
		for (uint32_t j = 0; j < height; j++)
		{
			this->cells[i][j] = Cell::EMPTY;
		}
	}
}
//TODO: needs to handle rotation
bool Board::TestCollision(Piece* piece)
{
	PieceDescription pd = pieceDictionary[uint32_t(piece->type)];
	for (uint32_t i = 0; i < 4; i++)
	{
		//TODO: needs to handle rotation
		Coord2D pieceCoord = piece->location + pd.offsets[i];
		Coord2D testCoord = pieceCoord + Coord2D(0, 1);
		if (testCoord.y >= Board::height)
		{
			return true;
		}
		if (cells[testCoord.x][testCoord.y] != Cell::EMPTY)
		{
			return true;
		}
	}
	return false;
}

//TODO: needs to handle rotation
void Board::SolidifyPiece(Piece* piece)
{
	PieceDescription pd = pieceDictionary[uint32_t(piece->type)];
	for (uint32_t i = 0; i < 4; i++)
	{
		//TODO: needs to handle rotation
		Coord2D pieceCoord = piece->location + pd.offsets[i];
		cells[pieceCoord.x][pieceCoord.y] = pd.color;
	}
}


Board::~Board()
{
}