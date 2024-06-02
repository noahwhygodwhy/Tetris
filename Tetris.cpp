#include <iostream>
#include <stdio.h>
#include <string>

#include <io.h>
#include <fcntl.h>

#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include "ostream"

#include "Board.hpp"
#include <optional>
#include <chrono>
#include <thread>

#include <unordered_map>

using namespace std::chrono;

using namespace std;

string spaces(uint32_t numSpaces)
{
    return string(numSpaces, ' ');
}

uint32_t ResetWindowSize(HANDLE hStdout)
{

    //cout << "\e[8;20;50t";

    short width = 30;
    short height = 50;
    
    SMALL_RECT gameWindowSize = SMALL_RECT{ 0, 0, width, height };
    COORD c = COORD{ width, height };

    if (!SetConsoleScreenBufferSize(hStdout, c))
    {
        printf("SetConsoleScreenBufferSize failed\n");
    }
    if (!SetConsoleWindowInfo(hStdout, true, &gameWindowSize))
    {
        printf("SetConsoleWindowInfo failed\n");
    }
    SetConsoleTitle(L"Tetris");
    return true;
}

static unordered_map<uint32_t, bool> keyDownMap;

#define INPUT_ENTRY(key, actionWhenDown) \
if ((GetKeyState(key) & 0x8000) && !keyDownMap[key])\
{\
    actionWhenDown;\
    keyDownMap[key] = true;\
}\
if ((!(GetKeyState(key) & 0x8000)) && keyDownMap[key])\
{\
    keyDownMap[key] = false;\
}

void TryMove(const Board& board, Piece* activePiece, Coord2D move)
{
    activePiece->location += move;
    if (board.TestOverlap(activePiece))
    {
        activePiece->location -= move;
    }
}
void TryRotate(const Board& board, Piece* activePiece, int rotation)
{
    int srcRotation = activePiece->rotation;
    activePiece->rotation = (activePiece->rotation + rotation) % 4; // rotation is a value between 0 and 3
    int dstRotation = activePiece->rotation;

    uint32_t key = wallKickKey(srcRotation, dstRotation);
    array<Coord2D, 5> possibleOffsets = activePiece->type == PieceType::I ? 
                                        IWallKicks.at(wallKickKey(srcRotation, dstRotation)) :
                                        GeneralWallKicks.at(wallKickKey(srcRotation, dstRotation)); 

    Coord2D originalLocation = activePiece->location;
    for (Coord2D offset : possibleOffsets)
    {
        activePiece->location = originalLocation + offset;
        if (!board.TestOverlap(activePiece))
        {
            // new rotation is good at the kick offset
            return;
        }

    }
    // fails rotation, reset piece
    activePiece->location = originalLocation;
    activePiece->rotation = srcRotation;

}

void HandleInput(const Board& board, Piece* activePiece)
{
    INPUT_ENTRY('A', TryMove(board, activePiece, Coord2D(-1, 0)))
    INPUT_ENTRY('D', TryMove(board, activePiece, Coord2D(1, 0)))
    INPUT_ENTRY('S', TryMove(board, activePiece, Coord2D(0, 1)))
    INPUT_ENTRY('Q', TryRotate(board, activePiece, -1))
    INPUT_ENTRY('E', TryRotate(board, activePiece, 1))
}




void Lose(HANDLE hStdout)
{
    SetConsoleTextAttribute(hStdout, 12);
    wprintf(L"\033[%d;%dH", (5), (Board::width + 4));
    wprintf(L"┌─────────────┐\n");
    wprintf(L"\033[%d;%dH", (6), (Board::width + 4));
    wprintf(L"│ You Lose :( │\n");
    wprintf(L"\033[%d;%dH", (7), (Board::width + 4));
    wprintf(L"└─────────────┘\n");
    wprintf(L"\033[%d;%dH", (Board::height + 3), (0));
    SetConsoleTextAttribute(hStdout, 15);
    system("pause");
    exit(0);
}



HANDLE hStdout;
int main()
{


    if (!SetConsoleOutputCP(CP_UTF8)) {
        printf("failed to set console output mode to unicode\n");
        return -1;
    }
    _setmode(_fileno(stdout), _O_U8TEXT);


    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_FONT_INFOEX cfi = { sizeof(CONSOLE_FONT_INFOEX) };

    // Populate cfi with the screen buffer's current font info

    if (!GetCurrentConsoleFontEx(hStdout, FALSE, &cfi))
    {
        wprintf(L"GetCurrentConsoleFontEx failed\n");
        exit(-1);
    }
    cfi.dwFontSize.Y = 16;
    cfi.dwFontSize.X = 16;
    //cfi.FaceName=
    //wprintf(L"%u, %u\n", cfi.dwFontSize.X, cfi.dwFontSize.Y);
    if (!SetCurrentConsoleFontEx(hStdout, TRUE, &cfi))
    {
        wprintf(L"SetCurrentConsoleFontEx failed\n");
        exit(-1);
    }


    //SetCurrentConsoleFontEx(hStdout, false, )
    //GetCurrentConsoleFontEx(hStdout, false, 0);

    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
    {
        printf("GetConsoleScreenBufferInfo (%d)\n", GetLastError());
        return 0;
    }

    CONSOLE_CURSOR_INFO curInfo;
    GetConsoleCursorInfo(hStdout, &curInfo);
    curInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hStdout, &curInfo);

    Board board = Board();
    PieceBag pieceBag = PieceBag();
    Piece * activePiece = nullptr;

    for (uint32_t i = 0; i < 0xFF; i++)
    {
        keyDownMap.insert(pair(i, false));
    }

    uint32_t score = 0;

    auto next = steady_clock::now();
    auto prev = next - 200ms;
    while (true)
    {
        bool tick = false;

        auto now = steady_clock::now();

        if (!activePiece)
        {
            activePiece = new Piece(pieceBag.next());
            if (board.TestOverlap(activePiece))
            {
                Lose(hStdout);
            }
            //pieceBag.next(activePiece));
        }

//INPUT HANDLING
        if (GetKeyState(VK_ESCAPE) & 0x8000)
        {
            break;
        }
        HandleInput(board, activePiece);
        
//GAME LOGIC

        if (abs(prev-now) >= 200ms)
        {
            tick = true;
            prev = now;
        }


        if (tick)
        {
            if (board.TestCollision(activePiece))
            {
                board.SolidifyPiece(activePiece);
                delete(activePiece);
                activePiece = nullptr;
            }
            else
            {
                activePiece->location += Coord2D(0, 1);
                //wprintf(L"activePieceLoc: %u, %u\n", activePiece->location.x, activePiece->location.y);
            }
            uint32_t linesEliminated = board.tick();
            uint32_t lineScore = 100u * pow(2u, linesEliminated - 1);
            score += lineScore;
        }
        
//DRAW STATE
//#define DEBUG_PRINT
#ifndef DEBUG_PRINT

        SetConsoleTextAttribute(hStdout, 15);
        wprintf(L"\033[%d;%dH", (0), (0));
        wprintf(L"╔");
        for (uint32_t i = 0; i < board.width; i++)
        {
            wprintf(L"═");
        }
        wprintf(L"╗");
        wprintf(L"\n");
        
        Coord2D topLeftOfBoard = Coord2D(0, 0);
        array<Coord2D, 4> pieceLocs = array<Coord2D, 4>();
        Coord2D boundingBoxMin = Coord2D(Board::width + 1, Board::height + 1);
        Coord2D boundingBoxMax = Coord2D(-1, -1);
        if (activePiece)
        {
            PieceDescription pd = pieceDictionary[uint32_t(activePiece->type)];
            for (uint32_t i = 0; i < 4; i++)
            {
                pieceLocs[i] = topLeftOfBoard + activePiece->location + activePiece->GetRotatedOffset(i);
                boundingBoxMax.maxWith(pieceLocs[i]);
                boundingBoxMin.minWith(pieceLocs[i]);
            }
        }
        for (uint32_t i = 0; i < board.height; i++)
        {
            wprintf(L"║");
            for (uint32_t j = 0; j < board.width; j++)
            {
                Coord2D loc = Coord2D(j, i);
                if (
                    (loc >= boundingBoxMin && loc <= boundingBoxMax) &&
                    std::find(pieceLocs.begin(), pieceLocs.end(), loc) != std::end(pieceLocs)
                )
                {
                    if (activePiece)
                    {
                        PieceDescription pd = pieceDictionary[uint32_t(activePiece->type)];
                        SetConsoleTextAttribute(hStdout, uint32_t(pd.color));
                        wprintf(L"█");
                        SetConsoleTextAttribute(hStdout, 15);
                    }
                    else
                    {
                        wprintf(L"active piece was null but it still hit its bounding box???\n");
                        exit(-1);
                    }
                }
                else if (board.cells[j][i] == Cell::EMPTY)
                {
                    wprintf(L" ");
                }
                else
                {
                    SetConsoleTextAttribute(hStdout, uint32_t(board.cells[j][i]));
                    wprintf(L"█");
                    SetConsoleTextAttribute(hStdout, 15);
                }
            }
            wprintf(L"║");
            wprintf(L"\n");
        }
        wprintf(L"╚");
        for (uint32_t i = 0; i < board.width; i++)
        {
            wprintf(L"═");
        }
        wprintf(L"╝");
        wprintf(L"\n");


        wprintf(L"\033[%d;%dH", (3), (Board::width + 4));
        wprintf(L"Score: %u\n", score);

#endif // DEBUG_PRINT
        next += 200ms;

    }
    wprintf(L"escape pressed, exiting\n");
}