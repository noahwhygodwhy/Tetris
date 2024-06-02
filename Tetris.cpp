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
    //printf("got console screen buffer info\n");
    //if (!ResetWindowSize(hStdout))
    //{
    //    printf("ResetWindowSize (%d)\n", GetLastError());
    //    return 0;
    //}
    //printf("did ResetWindowSize\n");
    //std::fflush(stdout);
    //exit(0);

    //for (uint32_t i = 0; i < 1000; i++)
    //{
    //    wprintf(L"%u: %c\n", i, char(i));
    //}
    //exit(0);
    Board board = Board();
    PieceBag pieceBag = PieceBag();
    Piece * activePiece = nullptr;

    unordered_map<uint32_t, bool> keyDownMap;
    for (uint32_t i = 0; i < 0xFF; i++)
    {
        keyDownMap.insert(pair(i, false));
    }

    uint32_t score = 0;

    //board.cells[5][5] = Cell::LIGHT_BLUE;
    //board.cells[5][6] = Cell::RED;
    //board.cells[0][0] = Cell::GREEN;
    //board.cells[board.width - 1][board.height - 1] = Cell::YELLOW;

    auto next = steady_clock::now();
    auto prev = next - 200ms;
    while (true)
    {
        auto now = steady_clock::now();

        if (!activePiece)
        {
            activePiece = new Piece(pieceBag.next());
            //pieceBag.next(activePiece));
        }

//INPUT HANDLING
        if (GetKeyState(VK_ESCAPE) & 0x8000)
        {
            break;
        }



        if ((GetKeyState('A')&0x8000) && !keyDownMap['A'])
        {
            activePiece->location += Coord2D(-1, 0);
            keyDownMap['A'] = true;
        }
        if ((!(GetKeyState('A') & 0x8000)) && keyDownMap['A'])
        {
            keyDownMap['A'] = false;
        }

        if ((GetKeyState('D') & 0x8000) && !keyDownMap['D'])
        {
            activePiece->location += Coord2D(1, 0);
            keyDownMap['D'] = true;
        }
        if ((!(GetKeyState('D') & 0x8000)) && keyDownMap['D'])
        {
            keyDownMap['D'] = false;
        }

//GAME LOGIC

        if (abs(prev-now) >= 200ms)
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

            //GAME TICK HERE

            prev = now;
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
                pieceLocs[i] = topLeftOfBoard + activePiece->location + pd.offsets[i];
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
#endif // DEBUG_PRINT
        next += 200ms;

    }
    wprintf(L"escape pressed, exiting\n");
}