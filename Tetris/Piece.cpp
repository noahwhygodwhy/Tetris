#include "Piece.hpp"
#include "Board.hpp"

Piece::Piece(PieceType pt)
{
	this->rotation = 0;
	this->type = pt;
	uint32_t bbSize = pieceDictionary[uint32_t(pt)].boundingBoxSize;
	uint32_t xLoc = bbSize % 2 == 0 ?
		((Board::width/2) - (bbSize / 2)) :
		((Board::width/2) - (bbSize / 2)) - 1;
	this->location = Coord2D(xLoc, 0);

	//this->location = bbSize %2 == 0? 
}

Coord2D Piece::GetRotatedOffset(uint32_t componentIndex)
{
	PieceDescription pd = pieceDictionary[uint32_t(this->type)];
	bool oddSized = pd.boundingBoxSize % 2 == 1;

	Coord2D original = pd.offsets[componentIndex];

	// bit funky but it works
	int32_t bboxSizeM1 = (pd.boundingBoxSize - 1);

	switch (this->rotation)
	{
	case 0:
		return original;
	case 1:
		return Coord2D(bboxSizeM1 - original.y, original.x);
	case 2:
		return Coord2D(bboxSizeM1 - original.x, bboxSizeM1 - original.y);
		break;
	case 3:
		return Coord2D(original.y, bboxSizeM1 - original.x);
		break;
	default:
		wprintf(L"rotation was a bad value: %u\n", this->rotation);
		exit(-1);
	}

	//if (oddSized)
	//{
	//}
	//else //even sized
	//{
	//	switch (this->rotation)
	//	{
	//	case 0:
	//		return original;
	//	case 1:

	//		break;
	//	case 2:
	//		break;
	//	case 3:
	//		break;
	//	default:
	//		wprintf(L"rotation was a bad value: %u\n", this->rotation);
	//	}
	//}
}
//void Piece::Draw(HANDLE hStdout, Coord2D topleftOfBoard)
//{
//	PieceDescription pd = pieceDictionary[uint32_t(this->type)];
//
//	SetConsoleTextAttribute(hStdout, uint32_t(this->type));
//	for (uint32_t i = 0; i < 4; i++)
//	{
//		Coord2D abLoc = topleftOfBoard + this->location + pd.offsets[i];
//		wprintf(L"\033[%d;%dH", abLoc.y, abLoc.x);
//		wprintf(L"█");
//	}
//	SetConsoleTextAttribute(hStdout, 15);
//}