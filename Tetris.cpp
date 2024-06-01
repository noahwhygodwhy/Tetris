#include <iostream>
#include <stdio.h>
#include <string>

#include <Windows.h>
#include <stdio.h>
#include <conio.h>

#include "Board.hpp"

using namespace std;

void drawState(const Board& board)
{

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
    //hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    //CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    //if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
    //{
    //    printf("GetConsoleScreenBufferInfo (%d)\n", GetLastError());
    //    return 0;
    //}
    //printf("got console screen buffer info\n");
    //if (!ResetWindowSize(hStdout))
    //{
    //    printf("ResetWindowSize (%d)\n", GetLastError());
    //    return 0;
    //}
    //printf("did ResetWindowSize\n");
    //std::fflush(stdout);
    //system("pause");
    //system("paus2");
    //exit(0);



    Board board = Board();
    uint32_t score = 0;

    while (true)
    {
//INPUT HANDLING
        if (GetKeyState(VK_ESCAPE) & 0x8000)
        {
            break;
        }
//GAME LOGIC

//DRAW STATE
        drawState(board);
    }

    printf("hello world\n");
}