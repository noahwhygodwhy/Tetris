#define WIN32_LEAN_AND_MEAN

#include "D2DContext.hpp"

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


#define INPUT_ENTRY_DOWN(key, actionWhenDown) \
if (lParam == key && !keyDownMap[key])\
{\
actionWhenDown; \
keyDownMap[key] = true; \
}


#define INPUT_ENTRY_UP(key) \
if (lParam == key && keyDownMap[key])\
{\
keyDownMap[key] = false; \
}


struct GameState
{
    Board board;
    PieceBag pieceBag;
    Piece* activePiece;
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    GameState* gz = (GameState*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    //DxrContext* ctx = (DxrContext*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    //WindowUserData* wud = (WindowUserData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    //TODO: make this userdata a lambda lmao

    switch (message)
    {
    case WM_KEYDOWN:
        INPUT_ENTRY_DOWN('A', TryMove(gz->board, gz->activePiece, Coord2D(-1, 0)))
        INPUT_ENTRY_DOWN('D', TryMove(gz->board, gz->activePiece, Coord2D(1, 0)))
        INPUT_ENTRY_DOWN('S', TryMove(gz->board, gz->activePiece, Coord2D(0, 1)))
        INPUT_ENTRY_DOWN('Q', TryRotate(gz->board, gz->activePiece, -1))
        INPUT_ENTRY_DOWN('E', TryRotate(gz->board, gz->activePiece, 1))
        if (lParam == VK_ESCAPE)
        {
            exit(0);
        }
        break;
    case WM_KEYUP:
        INPUT_ENTRY_UP('A')
        INPUT_ENTRY_UP('D')
        INPUT_ENTRY_UP('S')
        INPUT_ENTRY_UP('Q')
        INPUT_ENTRY_UP('E')
        break;
    case WM_CREATE:
        //OutputDebugStringF("create %u, height %u\n", LODWORD(lParam), HIDWORD(lParam));
        break;
    case WM_EXITSIZEMOVE:
    case WM_SIZE:
        return 0;
    break;
    case WM_PAINT:
        break;
    case WM_QUIT:
    case WM_DESTROY:
        exit(0);
        break;
    default:
        break;

    }    
    return DefWindowProc(hwnd, message, wParam, lParam);

    //return 0;
}



int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{

    const uint32_t blockSize = 45;
    const uint32_t blockpadding = 5;
    const uint32_t boardBorder = 10;
    const uint32_t numBlocksY = Board::height - 2;//cause top two boxes are invisible to player;
    const uint32_t numBlocksX = Board::width;

    const uint32_t boardBackgroundHeight = (numBlocksY * blockSize) + ((numBlocksY - 1) * blockpadding) + (boardBorder * 2);
    const uint32_t boardBackgroundWidth = (numBlocksX * blockSize) + ((numBlocksX - 1) * blockpadding) + (boardBorder * 2);

    const uint32_t boardPositionX = 0;
    const uint32_t boardPositionY = 0;

    const uint32_t defaultWidth = boardBackgroundWidth;
    const uint32_t defaultHeight = boardBackgroundHeight;
    WNDCLASSEX classDescriptor = {
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = WndProc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = hInstance,
        .hIcon = NULL,
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground = NULL,
        .lpszMenuName = L"",
        .lpszClassName = L"Tetris"
    };

    const LPWSTR registeredClass = (LPWSTR)RegisterClassEx(&classDescriptor);
    HWND hwnd = CreateWindow(registeredClass, L"", WS_OVERLAPPEDWINDOW/*WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX*/ | WS_VISIBLE, 0, 0, defaultWidth, defaultHeight, NULL, NULL, hInstance, NULL);
    
    
    
    D2DContext* ctx = new D2DContext(hwnd);

    ID2D1SolidColorBrush* whiteBrush = nullptr;
    ctx->d2d_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &whiteBrush);

    //The order of these matter, they line up with the Ordered enum PieceType
    array<ID2D1SolidColorBrush*, PieceType::COUNT> pieceBrushes;
    ctx->d2d_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Cyan),    &pieceBrushes[PieceType::I]);
    ctx->d2d_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow),  &pieceBrushes[PieceType::O]);
    ctx->d2d_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Magenta), &pieceBrushes[PieceType::T]);
    ctx->d2d_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green),   &pieceBrushes[PieceType::S]);
    ctx->d2d_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red),     &pieceBrushes[PieceType::Z]);
    ctx->d2d_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue),    &pieceBrushes[PieceType::J]);
    ctx->d2d_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Orange),  &pieceBrushes[PieceType::L]);


    //these in pixels


    //D2D1_RECT_F f = D2D1::RectF(5, 5, 500, 500);

    //ctx->d2d_renderTarget->FillRectangle(
    //    f,
    //    /*D2D1::RectF(
    //        ctx->rc->left + 100.0f,
    //        ctx->rc->top + 100.0f,
    //        ctx->rc->right - 100.0f,
    //        ctx->rc->bottom - 100.0f),*/
    //    pBlackBrush);
    GameState gz = {
        .board = Board(),
        .pieceBag = PieceBag(),
        .activePiece = nullptr
    };
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)&gz);

    for (uint32_t i = 0; i < 0xFF; i++)
    {
        keyDownMap.insert(pair(i, false));
    }

    uint32_t score = 0;

    auto next = steady_clock::now();
    auto prev = next - 200ms;

    MSG msg;
    bool running = true;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (running)
        {
            //GAME LOGIC
            bool tick = false;

            auto now = steady_clock::now();

            if (!gz.activePiece)
            {
                gz.activePiece = new Piece(gz.pieceBag.next());
                if (gz.board.TestOverlap(gz.activePiece))
                {
                    running = false;
                    continue;
                }
            }

            if (abs(prev - now) >= 200ms)
            {
                prev = now;

                if (gz.board.TestCollision(gz.activePiece))
                {
                    gz.board.SolidifyPiece(gz.activePiece);
                    delete(gz.activePiece);
                    gz.activePiece = nullptr;
                }
                else
                {
                    gz.activePiece->location += Coord2D(0, 1);
                }
                uint32_t linesEliminated = gz.board.tick();
                uint32_t lineScore = 100u * uint32_t(pow(2u, linesEliminated - 1u));
                score += lineScore;
            }

        }
        { // Draw section


            ctx->d2d_renderTarget->BeginDraw();
            ctx->d2d_renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

            D2D1_RECT_F background = D2D1_RECT_F(0 + boardPositionX, 0 + boardPositionY, boardBackgroundWidth + boardPositionX, boardBackgroundHeight + boardPositionY);
            ctx->d2d_renderTarget->FillRectangle(background, whiteBrush);


            TIF(ctx->d2d_renderTarget->EndDraw());
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

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
            }
            uint32_t linesEliminated = board.tick();
            uint32_t lineScore = 100u * uint32_t(pow(2u, linesEliminated - 1u));
            score += lineScore;
        }

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
        next += 200ms;
    }
    wprintf(L"escape pressed, exiting\n");
}