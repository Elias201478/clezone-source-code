#include <Windows.h>
#include <math.h>
#include "OtherPayloads.h"
#include "Definitions.h"


DWORD WINAPI ErrorRings(LPVOID lpThread) {
    int w = GetSystemMetrics(0);
    int h = GetSystemMetrics(1);
    HDC hdc = GetDC(0);
    HDC mem = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, w, h);
    SelectObject(mem, bmp);
    int radiusGrow = 2;
    int amount = rand() % 6;
    int Timeout = 1;

    static struct {
        int x, y;
        int r;
    } rings[1024];

    for (int i = 0; i < amount; i++) {
        rings[i].x = rand() % w;
        rings[i].y = rand() % h;
        rings[i].r = 0;
    }

    while (1) {

        for (int i = 0; i < amount; i++) {

            int x = rings[i].x;
            int y = rings[i].y;
            int rr = rings[i].r;

            for (float a = 0; a < 6.28318f; a += 0.35f) {

                int ix = x + (int)(cosf(a) * rr);
                int iy = y + (int)(sinf(a) * rr);

                int r = rand() % 4;
                HICON icon =
                    (r == 0) ? LoadIcon(NULL, IDI_ERROR) :
                    (r == 1) ? LoadIcon(NULL, IDI_WARNING) :
                    (r == 2) ? LoadIcon(NULL, IDI_INFORMATION) :
                    LoadIcon(NULL, IDI_QUESTION);

                DrawIconEx(hdc, ix - 16, iy - 16, icon, 32, 32, 0, 0, DI_NORMAL);
            }

            DrawIconEx(hdc, x - 16, y - 16, LoadIcon(NULL, IDI_ERROR), 32, 32, 0, 0, DI_NORMAL);

            
            rings[i].r += radiusGrow;

          
            if (rings[i].r > (w > h ? w : h)) {
                rings[i].r = 0;
                rings[i].x = rand() % w;
                rings[i].y = rand() % h;
            }
        }

        Sleep(Timeout);
        
    }
    return 0;
}

DWORD WINAPI RefreshScreen(LPVOID lpThread) {
    Sleep(1000);
    InvalidateRect(0, 0, 0);
}


float x = 300, y = 300;
float vx = 4, vy = 3;
int maxr = 80;

DWORD WINAPI BallThread(LPVOID lpThread) {
    HDC dc = GetDC(NULL);
    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);

    while (1) {
        x += vx;
        y += vy;

        if (x < maxr || x > sw - maxr) vx = -vx;
        if (y < maxr || y > sh - maxr) vy = -vy;

        for (int r = maxr; r > 0; r -= 10) {
            int c = (int)(128 + 127 * sin(GetTickCount() * 0.01 + r));
            HBRUSH b = CreateSolidBrush(RGB(c, 0, 0));
            HBRUSH o = SelectObject(dc, b);
            Ellipse(dc, (int)(x - r), (int)(y - r), (int)(x + r), (int)(y + r));
            SelectObject(dc, o);
            DeleteObject(b);
        }

        Sleep(1);
    }
    return 0;
}