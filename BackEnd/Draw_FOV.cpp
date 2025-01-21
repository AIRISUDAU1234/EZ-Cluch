#include <windows.h>
#include "Global.h"

HWND hwnd;

void Invalidate(HWND hwnd);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(hwnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        int centerX = width / 2;
        int centerY = height / 2;

        // 描画設定
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH)); 
        HPEN hNewPen = (HPEN)CreatePen(PS_INSIDEFRAME, 1, RGB(0x00, 0xFF, 0xFF)); 
        HPEN hOldPen = (HPEN)SelectObject(hdc, hNewPen);

        Ellipse(hdc, centerX - CrossRadius, centerY - CrossRadius, centerX + CrossRadius, centerY + CrossRadius);

        SelectObject(hdc, hOldBrush);
        SelectObject(hdc, hOldPen);
        DeleteObject(hNewPen);

        EndPaint(hwnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

void Draw_FOV() 
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"cluch";
    RegisterClass(&wc);

    // 画面サイズを取得(取得方法変更するかも)
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    HWND hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW, 
        wc.lpszClassName,
        L"",
        WS_POPUP, 
        0, 0, screenWidth, screenHeight, 
        NULL, NULL, hInstance, NULL);

    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}


void Radius_Reload() //外部からの再描画呼び出し
{
    Invalidate(hwnd);
}

void Invalidate(HWND hwnd) //再描画
{
    InvalidateRect(hwnd, NULL, TRUE);  
}

void ExitOverlay() //OFF
{
    Exit(hwnd);
}

void Exit(HWND hwnd) 
{
    DestroyWindow(hwnd);
    PostQuitMessage(0);
}