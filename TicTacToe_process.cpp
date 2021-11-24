/*
 * Dependencies:
 *  gdi32
 *  (kernel32)
 *  user32
 *  (comctl32)
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>
#include <vector>
#include <fstream>
#include <Windowsx.h>
#include <winuser.h>
#include <winbase.h>
#include <memoryapi.h>

using namespace std;

#define KEY_SHIFTED     0x8000
#define KEY_TOGGLED     0x0001

const TCHAR szWinClass[] = _T("Win32SampleApp");
const TCHAR szWinName[] = _T("Semenov_PMI_2");
HWND hwnd;               /* This is the handle for our window */
HBRUSH hBrush;           /* Current brush */
HPEN penGrid, penCross, penCircle;
//rgb сетки
int gc1 = 255;
int gc2 = 0;
int gc3 = 0;
// rgb фона
int bc1 = 0;
int bc2 = 0;
int bc3 = 255;
int width = 320; //ширина окна
int height = 240; //высота окна
int clientWidth; //ширина клиентской области
int clientHeight; //высота клиентской области
int n = 3; // размерность сетки
int* mas = new int[n * n]; //двумерный массив в виде одномерного
UINT myMsg; // широковещательноее сообщение

void MakeLine(HDC& hdc, HPEN hPen, int x0, int y0, int x1, int y1) {
    SelectObject(hdc, hPen);
    MoveToEx(hdc, x0, y0, NULL);
    LineTo(hdc, x1, y1);
}

void MakeCircle(HDC& hdc, HPEN hPen, int left, int top, int right, int bottom) {
    SelectObject(hdc, hPen);
    Ellipse(hdc, left, top, right, bottom);
}

/* Runs Notepad */
void RunNotepad(void)
{
    STARTUPINFO sInfo;
    PROCESS_INFORMATION pInfo;

    ZeroMemory(&sInfo, sizeof(STARTUPINFO));

    puts("Starting Notepad...");
    CreateProcess(_T("C:\\Windows\\Notepad.exe"),
        NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo);
}

/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int x, y, masX, masY;
    PAINTSTRUCT ps;
    HDC hdc;
    RECT windowRect;

    //обработка сообщения myMsg
    if (message == myMsg) {
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    }

    switch (message)                  /* handle the messages */
    {

    case WM_CREATE:
        //регистрируем сообщение для обновления окон
        myMsg = RegisterWindowMessage(_T("WindowsUpdate"));
        break;

    case WM_SIZE:
    {
        GetWindowRect(hwnd, &windowRect);
        width = windowRect.right - windowRect.left;
        height = windowRect.bottom - windowRect.top;
        clientWidth = LOWORD(lParam);
        clientHeight = HIWORD(lParam);
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }

    case WM_PAINT:
    {
        hdc = BeginPaint(hwnd, &ps);
        //отрисовка сетки
        for (int i = 1; i < n; i++) {
            MakeLine(hdc, penGrid, (clientWidth / n) * i, 0, (clientWidth / n) * i, clientHeight);
            MakeLine(hdc, penGrid, 0, (clientHeight / n) * i, clientWidth, (clientHeight / n) * i);
        }

        //отрисовка кружков
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (mas[i * n + j] == 1) {
                    MakeCircle(hdc, penCircle, ((clientWidth / n) * i) + 10, ((clientHeight / n) * j) + 10, (clientWidth / n) * (i + 1) - 10, (clientHeight / n) * (j + 1) - 10);
                }
            }
        }

        //отрисовка крестиков
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (mas[i * n + j] == 2) {
                    MakeLine(hdc, penCross, (clientWidth / n) * i + 10, (clientHeight / n) * j + 10, (clientWidth / n) * (i + 1) - 10, (clientHeight / n) * (j + 1) - 10);
                    MakeLine(hdc, penCross, (clientWidth / n) * (i + 1) - 10, (clientHeight / n) * j + 10, (clientWidth / n) * i + 10, (clientHeight / n) * (j + 1) - 10);
                }
            }
        }
        EndPaint(hwnd, &ps);
        DeleteObject(hdc);
        break;
    }

    case WM_LBUTTONDOWN:
        //получаем координаты клика мыши
        x = GET_X_LPARAM(lParam);
        y = GET_Y_LPARAM(lParam);
        //вычисляем нужный блок сетки
        masX = (int)(x / (clientWidth / n));
        masY = (int)(y / (clientHeight / n));
        //заполняем массив
        if (mas[masX * n + masY] == 0) mas[masX * n + masY] = 1;

        //создаем сообщение при нажатии лкм
        PostMessage(HWND_BROADCAST, myMsg, wParam, lParam);

        InvalidateRect(hwnd, NULL, TRUE);
        break;

    case WM_RBUTTONDOWN:
        //получаем координаты клика мыши
        x = GET_X_LPARAM(lParam);
        y = GET_Y_LPARAM(lParam);
        //вычисляем нужный блок сетки
        masX = (int)(x / (clientWidth / n));
        masY = (int)(y / (clientHeight / n));
        //заполняем массив
        if (mas[masX * n + masY] == 0) mas[masX * n + masY] = 2;

        //создаем сообщение при нажатии пкм
        PostMessage(HWND_BROADCAST, myMsg, wParam, lParam);

        InvalidateRect(hwnd, NULL, TRUE);
        break;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) {
            PostQuitMessage(0);
        }

        //меняем цвет фона на рандомный
        if (wParam == VK_RETURN) {
            bc1 = rand() % 100;
            bc2 = rand() % 100;
            bc3 = rand() % 100;
            if (hBrush) {
                DeleteObject(hBrush);
            }
            hBrush = CreateSolidBrush(RGB(bc1, bc2, bc3));
            SetClassLongPtr(hwnd, GCL_HBRBACKGROUND, (LONG)hBrush);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        break;

    case WM_HOTKEY:
        if (wParam == 1) {
            PostQuitMessage(0);
        }

        if (wParam == 2) {
            RunNotepad();
        }
        break;

    case WM_MOUSEWHEEL: {
        gc1 = (gc1 + rand() % 10) % 255;
        gc2 = (gc2 + rand() % 10) % 255;
        gc3 = (gc2 + rand() % 10) % 255;
        penGrid = CreatePen(PS_SOLID, 3, RGB(gc1, gc2, gc3));
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
        return 0;
    }
    /* for messages that we don't deal with */
    return DefWindowProc(hwnd, message, wParam, lParam);
}

int main(int argc, char** argv)
{
    string filename = "config.txt";
    ifstream file(filename);
    if (file.is_open()) {
        file >> width >> height >> n >> bc1 >> bc2 >> bc3 >> gc1 >> gc2 >> gc3;
        file.close();
    }
    if (argc == 3) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
    }
    if (argc == 4) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
        n = atoi(argv[3]);
    }

    if (n != 3) {
        delete[]mas;
        mas = new int[n * n];
    }

    HANDLE lpFile;
    //Создаем разделенный участок памяти
    lpFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_EXECUTE_READWRITE, 0, (n * n) * sizeof(int), "lpfilename");
    //создаем окно просмотров
    mas = (int*)MapViewOfFile(lpFile, FILE_MAP_ALL_ACCESS, 0, 0, (n * n) * sizeof(int));


    BOOL bMessageOk;
    MSG message;            /* Here message to the application are saved */
    WNDCLASS wincl = { 0 };         /* Data structure for the windowclass */

    /* Harcode show command num when use non-winapi entrypoint */
    int nCmdShow = SW_SHOW;
    /* Get handle */
    HINSTANCE hThisInstance = GetModuleHandle(NULL);

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szWinClass;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by Windows */

    /* Use custom brush to paint the background of the window */
    hBrush = CreateSolidBrush(RGB(bc1, bc2, bc3));
    penGrid = CreatePen(PS_SOLID, 3, RGB(gc1, gc2, gc3)); // цвет для сетки
    penCross = CreatePen(PS_SOLID, 5, RGB(0, 255, 0)); // цвет для крестиков
    penCircle = CreatePen(PS_SOLID, 7, RGB(0, 0, 0)); // цвет для кружков
    wincl.hbrBackground = hBrush;


    /* Register the window class, and if it fails quit the program */
    if (!RegisterClass(&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindow(
        szWinClass,          /* Classname */
        szWinName,       /* Title Text */
        WS_OVERLAPPEDWINDOW, /* default window */
        CW_USEDEFAULT,       /* Windows decides the position */
        CW_USEDEFAULT,       /* where the window ends up on the screen */
        width,                 /* The programs width */
        height,                 /* and height in pixels */
        HWND_DESKTOP,        /* The window is a child-window to desktop */
        NULL,                /* No menu */
        hThisInstance,       /* Program Instance handler */
        NULL                 /* No Window Creation data */
    );

    /* Make the window visible on the screen */
    ShowWindow(hwnd, nCmdShow);

    //UpdateWindow(hwnd);

    RegisterHotKey(hwnd, 1, MOD_CONTROL, 0x51); // CTRL + Q
    RegisterHotKey(hwnd, 2, MOD_SHIFT, 0x43); // CTRL + C

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while ((bMessageOk = GetMessage(&message, NULL, 0, 0)) != 0)
    {
        /* Yep, fuck logic: BOOL mb not only 1 or 0.
         * See msdn at https://msdn.microsoft.com/en-us/library/windows/desktop/ms644936(v=vs.85).aspx
         */
        if (bMessageOk == -1)
        {
            puts("Suddenly, GetMessage failed! You can call GetLastError() to see what happend");
            break;
        }
        /* Translate virtual-key message into character message */
        TranslateMessage(&message);
        /* Send message to WindowProcedure */
        DispatchMessage(&message);
    }

    /* Cleanup stuff */
    DestroyWindow(hwnd);
    UnregisterClass(szWinClass, hThisInstance);
    DeleteObject(hBrush);
    DeleteObject(penGrid);
    DeleteObject(penCross);
    ofstream fileres(filename);
    fileres << width << ' ' << height << ' ' << n << ' ' << bc1 << ' ' << bc2 << ' ' << bc3 << ' ' << gc1 << ' ' << gc2 << ' ' << gc3;
    fileres.close();
    UnmapViewOfFile(lpFile);
    CloseHandle(lpFile);
    return 0;
}

