#pragma once

#include <array>
#include <random>
#include <algorithm>
#include "Windows.h"
#include <unordered_map>
using namespace std;

// all coords are assuming the vertex top left is (0, 0)
// and if the coord is used to describe a square's location
// then th square extends down right
struct Coord2D
{
    int32_t x;
    int32_t y;
    Coord2D() : x(-1), y(-1) {}
    Coord2D(uint32_t x, uint32_t y) : x(x), y(y) {}
    Coord2D operator+(Coord2D other)
    {
        Coord2D retVal(x, y);
        retVal.x += other.x;
        retVal.y += other.y;
        return retVal;
    }
    void operator+=(Coord2D other)
    {
        this->x += other.x;
        this->y += other.y;
    }
    void operator-=(Coord2D other)
    {
        this->x -= other.x;
        this->y -= other.y;
    }
    bool operator==(Coord2D other)
    {
        return this->x == other.x && this->y == other.y;
    }
    void maxWith(Coord2D other)
    {
        this->x = max(this->x, other.x);
        this->y = max(this->y, other.y);
    }
    void minWith(Coord2D other)
    {
        this->x = min(this->x, other.x);
        this->y = min(this->y, other.y);
    }
    bool operator>(Coord2D other)
    {
        return (this->x > other.x) && (this->y > other.y);
    }
    bool operator>=(Coord2D other)
    {
        return (this->x >= other.x) && (this->y >= other.y);
    }
    bool operator<(Coord2D other)
    {
        return (this->x < other.x) && (this->y < other.y);
    }
    bool operator<=(Coord2D other)
    {
        return (this->x <= other.x) && (this->y <= other.y);
    }
};


inline uint32_t wallKickKey(int32_t srcRotation, int32_t dstRotation)
{
    //src and dst rotation will always be values between 0 and 3
    return srcRotation | (dstRotation << 2);
}

// based on data from https://tetris.fandom.com/wiki/SRS
// important to note that the y values are inverted 
// cause in the coordinate system I used, y=0 is the top of the board
// apparently that was wrong lmao
static const unordered_map<uint32_t, array<Coord2D, 5>> GeneralWallKicks =
{
    { wallKickKey(0, 1), {Coord2D(0, 0), Coord2D(-1, 0), Coord2D(-1, -1), Coord2D(0, 2), Coord2D(-1, -2) }},
    { wallKickKey(1, 0), {Coord2D(0, 0), Coord2D(1, 0), Coord2D(-1, 1), Coord2D(0, -2), Coord2D(1, -2) }},
    { wallKickKey(1, 2), {Coord2D(0, 0), Coord2D(1, 0), Coord2D(1, 1), Coord2D(0, -2), Coord2D(1, -2) }},
    { wallKickKey(2, 1), {Coord2D(0, 0), Coord2D(-1, 0), Coord2D(-1, -1), Coord2D(0, 2), Coord2D(-1, 2) }},
    { wallKickKey(2, 3), {Coord2D(0, 0), Coord2D(1, 0), Coord2D(1, -1), Coord2D(0, 2), Coord2D(1, 2) }},
    { wallKickKey(3, 2), {Coord2D(0, 0), Coord2D(-1, 0), Coord2D(-1, 1), Coord2D(0, -2), Coord2D(-1, -2) }},
    { wallKickKey(3, 0), {Coord2D(0, 0), Coord2D(-1, 0), Coord2D(-1, 1), Coord2D(0, -2), Coord2D(-1, -2) }},
    { wallKickKey(0, 3), {Coord2D(0, 0), Coord2D(1, 0), Coord2D(1, -1), Coord2D(0, 2), Coord2D(1, 2) }},
};

//why is I so special :|

static const unordered_map<uint32_t, array<Coord2D, 5>> IWallKicks =
{
    { wallKickKey(0, 1), {Coord2D(0, 0), Coord2D(-2, 0), Coord2D(1, 0), Coord2D(-2, 1), Coord2D(1, -2) }},
    { wallKickKey(1, 0), {Coord2D(0, 0), Coord2D(2, 0), Coord2D(-1, 0), Coord2D(2, -1), Coord2D(-1, 2) }},
    { wallKickKey(1, 2), {Coord2D(0, 0), Coord2D(-1, 0), Coord2D(2, 0), Coord2D(-1, -2), Coord2D(2, 1) }},
    { wallKickKey(2, 1), {Coord2D(0, 0), Coord2D(1, 0), Coord2D(-2, 0), Coord2D(1, 2), Coord2D(-2, -1) }},
    { wallKickKey(2, 3), {Coord2D(0, 0), Coord2D(2, 0), Coord2D(-1, 0), Coord2D(2, -1), Coord2D(-1, 2) }},
    { wallKickKey(3, 2), {Coord2D(0, 0), Coord2D(-2, 0), Coord2D(1, 0), Coord2D(-2, 1), Coord2D(1, -2) }},
    { wallKickKey(3, 0), {Coord2D(0, 0), Coord2D(1, 0), Coord2D(-2, 0), Coord2D(1, 2), Coord2D(-2, -1) }},
    { wallKickKey(0, 3), {Coord2D(0, 0), Coord2D(-1, 0), Coord2D(2, 0), Coord2D(-1, -2), Coord2D(2, 1) }},
};

enum class Cell : uint32_t
{
    EMPTY = 0,
    LIGHT_BLUE = 11,
    DARK_BLUE = 9,
    ORANGE = 6,
    YELLOW = 14,
    GREEN = 10,
    RED = 12,
    MAGENTA = 13,
    COUNT = 8,
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
 

struct PieceDescription
{
    uint32_t boundingBoxSize;
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
/*L*/ PieceDescription{3, {Coord2D(2, 0), Coord2D(0, 1), Coord2D(1, 1), Coord2D(2, 1)}, Cell::ORANGE},
};

class Piece
{
public:
    Coord2D location = Coord2D(0, 0); //top left of the pieces bounding box (not strictly a "filled" square)
    PieceType type;
    uint32_t rotation = 0; //0 to 3, what this actually means tbd

    Piece(PieceType pt);

    //void Draw(HANDLE hStdout, Coord2D topleftOfBoard);
    Coord2D GetRotatedOffset(uint32_t componentIndex);
};

struct PieceBag
{
    array<PieceType, uint32_t(PieceType::COUNT)> queue = array<PieceType, uint32_t(PieceType::COUNT)>();
    uint32_t currIndex = uint32_t(PieceType::COUNT);

    std::random_device rd;
    std::mt19937 g = mt19937(rd());

    void shuffleIt()
    {
        for (uint32_t i = 0; i < uint32_t(PieceType::COUNT); i++)
        {
            queue[i] = PieceType(i);
        }
        shuffle(queue.begin(), queue.end(), g);
    }
    PieceType next()
    {
        if (currIndex >= uint32_t(PieceType::COUNT))
        {
            currIndex = 0;
            this->shuffleIt();
        }
        return queue[currIndex++];
    }
    
};
