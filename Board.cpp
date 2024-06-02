// score
// 10x20 cells
// next piece preview
// hold piece location
#include "Board.hpp"
#include "Piece.hpp"

Board::Board()
{
	this->empty = array<bool, height>();
	for (uint32_t j = 0; j < height; j++)
	{
		this->empty[j] = true;
	}
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
bool Board::TestCollision(Piece* piece) const
{
	for (uint32_t i = 0; i < 4; i++)
	{
		//TODO: needs to handle rotation
		Coord2D pieceCoord = piece->location + piece->GetRotatedOffset(i);
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
bool Board::TestOverlap(Piece* piece) const
{
	for (uint32_t i = 0; i < 4; i++)
	{
		//TODO: needs to handle rotation
		Coord2D pieceCoord = piece->location + piece->GetRotatedOffset(i);
		//test bottom
		if (pieceCoord.x >= Board::width || pieceCoord.x < 0 || pieceCoord.y >= Board::height)
		{
			return true;
		}
		if (cells[pieceCoord.x][pieceCoord.y] != Cell::EMPTY)
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
		Coord2D pieceCoord = piece->location + piece->GetRotatedOffset(i);
		cells[pieceCoord.x][pieceCoord.y] = pd.color;
		empty[pieceCoord.y] = false;
	}
}



uint32_t Board::tick()
{

	uint32_t numRows = 0;

	uint32_t bottomRow = Board::height - 1;


	bool rowempty = true;

	int32_t firstEmpty = -1;
	bool anythingToMoveDown = false;
	for (int32_t row = Board::height-1; row >= 0; row--)
	{
		if (empty[row] && firstEmpty == -1)
		{
			//identification of the first empty
			firstEmpty = row;
		}
		else if (firstEmpty != -1 && empty[row] == false)
		{
			anythingToMoveDown = true;
		}
	}

	if (anythingToMoveDown)
	{
		//move everything down one
		for (uint32_t x = 0; x < Board::width; x++)
		{
			for (int32_t y = firstEmpty; y >= 0; y--)
			{
				if (y == 0)
				{
					cells[x][y] = Cell::EMPTY;
				}
				else
				{
					cells[x][y] = cells[x][y - 1];
				}
			}
		}
		for (int32_t y = firstEmpty; y >= 0; y--)
		{
			if (y == 0)
			{
				empty[y] = true;
			}
			else
			{
				empty[y] = empty[y - 1];
			}
		}
		return 0;
	}


	//for (uint32_t col = 0; col < Board::width; col++)
	//{
	//	if (cells[col][bottomRow] != Cell::EMPTY)
	//	{
	//		rowempty = false;
	//		break;
	//	}
	//}
	//if (rowempty)
	//{
	//	//move everything down one
	//	for (uint32_t x = 0; x < Board::width; x++)
	//	{
	//		for (int32_t y = Board::height-1; y >= 0; y-- )
	//		{
	//			if (y == 0)
	//			{
	//				cells[x][y] = Cell::EMPTY;
	//			}
	//			else
	//			{
	//				cells[x][y] = cells[x][y - 1];
	//			}
	//		}
	//	}
	//	return 0; //tetris
	//}

	bool hadAFilledRow = false;
	for (int32_t rowIndex = Board::height - 1; rowIndex >= 0; rowIndex--)
	{
		bool rowfilled = true;
		for (uint32_t col = 0; col < Board::width; col++)
		{
			if (cells[col][rowIndex] == Cell::EMPTY)
			{
				rowfilled = false;
				break;
			}
		}
		if (!rowfilled && hadAFilledRow)
		{
			break;
		}
		else if (rowfilled)
		{
			hadAFilledRow = true;
			numRows++;
			for (uint32_t col = 0; col < Board::width; col++)
			{
				cells[col][rowIndex] = Cell::EMPTY;
			}
			empty[rowIndex] = true;

		}
	}
	return numRows;
}



Board::~Board()
{
}