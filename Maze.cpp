#include "framework.h"
#include "Maze.h"
#include<iostream>
#include<vector>
#include<stack>
#include<ctime>
#include<thread>
#define MAX_LOADSTRING 100
#define Top 0
#define Right 1
#define Bottom 2
#define Left 3
// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];
HWND mHwnd;
RECT clientRect;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

class Cell;

const size_t WIDTHw = 817;
const size_t HEIGHTw = 540;
const size_t TILE = 50;
std::vector<Cell> gridCells;
bool Game = true;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MAZE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAZE));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

class Cell {
    HDC hdc;
public:
    int x;
    int y;
    bool walls[4];
    bool visited;
    size_t COLS, ROWS;
    Cell(HDC hdc, int x, int y, size_t ROWS, size_t COLS) {
        this->x = x;
        this->y = y;
        this->hdc = hdc;
        this->COLS = COLS;
        this->ROWS = ROWS;
        std::memset(walls, 1, sizeof(walls));
        visited = false;
    }
    void draw() {
        int xd = this->x * TILE;
        int yd = this->y * TILE;
        if (visited) {
            HGDIOBJ oldPen = SelectObject(hdc, GetStockObject(NULL_PEN));
            Rectangle(hdc, xd, yd, xd + TILE, yd + TILE);
            SelectObject(hdc, oldPen);
        }

        if (walls[Top]) {
            MoveToEx(hdc, xd, yd, NULL);
            HPEN newPen = CreatePen(PS_SOLID, 1, RGB(209, 113, 113));
            HGDIOBJ oldPen = SelectObject(hdc, newPen);
            LineTo(hdc, xd + TILE, yd);
            DeleteObject(newPen);
        }
        else {
            HGDIOBJ oldPen = SelectObject(hdc, GetStockObject(NULL_PEN));
            Rectangle(hdc, xd + 1, yd - TILE + 1, xd + 1, yd + TILE);
            SelectObject(hdc, oldPen);
        }

        if (walls[Right]) {
            MoveToEx(hdc, xd + TILE, yd, NULL);
            HPEN newPen = CreatePen(PS_SOLID, 1, RGB(209, 113, 113));
            HGDIOBJ oldPen = SelectObject(hdc, newPen);
            LineTo(hdc, xd + TILE, yd + TILE);
            DeleteObject(newPen);
        }
        else {
            HGDIOBJ oldPen = SelectObject(hdc, GetStockObject(NULL_PEN));
            Rectangle(hdc, xd + 1, yd + 1, xd + TILE * 2, yd + TILE);
            SelectObject(hdc, oldPen);
        }

        if (walls[Bottom]) {
            MoveToEx(hdc, xd, yd + TILE, NULL);
            HPEN newPen = CreatePen(PS_SOLID, 1, RGB(209, 113, 113));
            HGDIOBJ oldPen = SelectObject(hdc, newPen);
            LineTo(hdc, xd + TILE, yd + TILE);
            DeleteObject(newPen);
        }
        else {
            HGDIOBJ oldPen = SelectObject(hdc, GetStockObject(NULL_PEN));
            Rectangle(hdc, xd + 1, yd + 1, xd + TILE, yd + TILE * 2);
            SelectObject(hdc, oldPen);
        }

        if (walls[Left]) {
            MoveToEx(hdc, xd, yd, NULL);
            HPEN newPen = CreatePen(PS_SOLID, 1, RGB(209, 113, 113));
            HGDIOBJ oldPen = SelectObject(hdc, newPen);
            LineTo(hdc, xd, yd + TILE);
            DeleteObject(newPen);
        }
        else {
            HGDIOBJ oldPen = SelectObject(hdc, GetStockObject(NULL_PEN));
            Rectangle(hdc, xd - TILE + 1, yd + 1, xd + TILE, yd + TILE);
            SelectObject(hdc, oldPen);
        }

    }
    void drawCurrent() {
        int xd = this->x * TILE;
        int yd = this->y * TILE;
        HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(LTGRAY_BRUSH));
        HGDIOBJ oldPen = SelectObject(hdc, GetStockObject(NULL_PEN));
        Rectangle(hdc, xd + 1, yd + 1, xd + TILE, yd + TILE);
        SelectObject(hdc, oldBrush);
    }
    Cell checkCell(int x, int y) {
        short findIndex = x + y * COLS;
        if (x < 0 or x > COLS - 1 or y < 0 or y > ROWS - 1) {
            Cell a(0, 0, 0, 0, 0);
            return a;
        }
        return gridCells[findIndex];
    }
    Cell checkNeighbors() {
        std::vector<Cell> neighbors;
        Cell top = checkCell(x, y - 1);
        Cell right = checkCell(x + 1, y);
        Cell bottom = checkCell(x, y + 1);
        Cell left = checkCell(x - 1, y);
        if (top.COLS != 0 and !top.visited) {
            neighbors.push_back(top);
        }
        if (right.COLS != 0 and !right.visited) {
            neighbors.push_back(right);
        }
        if (bottom.COLS != 0 and !bottom.visited) {
            neighbors.push_back(bottom);
        }
        if (left.COLS != 0 and !left.visited) {
            neighbors.push_back(left);
        }
        if (neighbors.size()) {
            return neighbors[rand() % neighbors.size()];
        }
        else {
            Cell a(0, 0, 0, 0, 0);
            return a;
        }
    }
};

void removeWalls(Cell& current, Cell& next) {
    int dx = current.x - next.x;
    if (dx == 1) {
        current.walls[Left] = false;
        next.walls[Right] = false;
    }
    else if (dx == -1) {
        current.walls[Right] = false;
        next.walls[Left] = false;
    }
    int dy = current.y - next.y;
    if (dy == 1) {
        current.walls[Top] = false;
        next.walls[Bottom] = false;
    }
    else if (dy == -1) {
        current.walls[Bottom] = false;
        next.walls[Top] = false;
    }
}

void Paipai(HDC hdc, size_t ROWS, size_t COLS, HWND hwnd, PAINTSTRUCT& ps) {
    srand(time(0));
    FillRect(hdc, &clientRect, reinterpret_cast<HBRUSH>(GetStockObject(DKGRAY_BRUSH)));
    HPEN newPen = CreatePen(PS_SOLID, 1, RGB(209, 113, 113));
    HGDIOBJ oldPen = SelectObject(hdc, newPen);
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(BLACK_BRUSH));
    for (size_t i = 0; i < ROWS; ++i) {
        for (size_t j = 0; j < COLS; ++j) {
            Cell a(hdc, j, i, ROWS, COLS);
            gridCells.push_back(a);
        }
    }
    Cell& currentCell = gridCells[0];
    std::stack<Cell> ss;
    while (Game) {
        for (size_t i = 0; i < gridCells.size(); ++i) {
            gridCells[i].draw();
        }
        currentCell.visited = true;
        currentCell.drawCurrent();

        Cell nextCell = currentCell.checkNeighbors();
        if (nextCell.COLS != 0) {
            gridCells[nextCell.x + nextCell.y * COLS].visited = true;
            ss.push(currentCell);
            removeWalls(gridCells[currentCell.x + currentCell.y * COLS], gridCells[nextCell.x + nextCell.y * COLS]);
            currentCell = nextCell;
        }
        else if (ss.size()) {
            currentCell = ss.top();
            ss.pop();
        }
        else if (ss.empty()) {
            Game = false;
        }
    }
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(newPen);

}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAZE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 

    HWND hWnd = CreateWindowW(szWindowClass, L"Maze", WS_OVERLAPPEDWINDOW,
        400, 200, WIDTHw, HEIGHTw, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        GetClientRect(hWnd, &clientRect);
        size_t WIDTH = clientRect.right;
        size_t HEIGHT = clientRect.bottom;
        size_t COLS = WIDTH / TILE;
        size_t ROWS = HEIGHT / TILE;
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);


        srand(time(0));
        Paipai(hdc, ROWS, COLS, hWnd, ps);

        EndPaint(hWnd, &ps);

    }
    break;
    case WM_KEYDOWN:
        switch (LOWORD(wParam)) {
        case VK_ESCAPE:
            Game = false;
        }
        break;
    case WM_GETMINMAXINFO:
    {
        LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
        lpMMI->ptMinTrackSize.x = 817;
        lpMMI->ptMinTrackSize.y = 540;

        lpMMI->ptMaxTrackSize.x = 817;
        lpMMI->ptMaxTrackSize.y = 540;
    }
    break;
    case WM_NCHITTEST:
    {
        LRESULT lRes = DefWindowProc(hWnd, WM_NCHITTEST, wParam, lParam);
        if (lRes == HTCAPTION)
            lRes = HTCLIENT;
        return lRes;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


