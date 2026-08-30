#include <windows.h>
#include "GDI.h"
#include "Definitions.h"
#include <math.h>
#include <stdint.h>

#pragma comment(lib, "msimg32.lib")

#define PAY 3.1415926535f
int ShiftSpeed5 = 3;

DWORD WINAPI tHSL(LPVOID lpThread) {
    HDC sdc = GetDC(NULL);
    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);

    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = sw;
    bi.bmiHeader.biHeight = -sh;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    RGBQUAD* buf;
    HBITMAP bmp = CreateDIBSection(sdc, &bi, DIB_RGB_COLORS, (void**)&buf, NULL, 0);
    HDC mdc = CreateCompatibleDC(sdc);
    SelectObject(mdc, bmp);

    float shift = 0.0f;

    while (1) {
        BitBlt(mdc, 0, 0, sw, sh, sdc, 0, 0, SRCCOPY);

        shift += 1.0f;
        if (shift >= 360.0f) shift -= 360.0f;

        int count = sw * sh;

        for (int i = 0; i < count; i++) {
            HSL hsl = rgb2hsl(buf[i]);

            hsl.h += shift;
            if (hsl.h >= 360.0f) hsl.h -= 360.0f;
            if (hsl.h < 0.0f) hsl.h += 360.0f;

            buf[i] = hsl2rgb(hsl);
        }

        BitBlt(sdc, 0, 0, sw, sh, mdc, 0, 0, SRCCOPY);
        Sleep(1);
    }

    return 0;
}




DWORD WINAPI warp(LPVOID lpThread) {
    int W = GetSystemMetrics(SM_CXSCREEN);
    int H = GetSystemMetrics(SM_CYSCREEN);

    HDC screen = GetDC(NULL);
    HDC memdc = CreateCompatibleDC(screen);
    HBITMAP bmp = CreateCompatibleBitmap(screen, W, H);
    SelectObject(memdc, bmp);

    float t = 0;

    while (1)
    {
        BitBlt(memdc, 0, 0, W, H, screen, 0, 0, SRCCOPY);

        for (int y = 0; y < H; y++)
        {
            float warp = sinf(y * 0.06f + t) * 20.0f;

            BitBlt(
                screen,
                (int)warp, y, W, 1,
                memdc,
                0, y,
                SRCCOPY
            );
        }

        t += 0.05f;
        Sleep(1);
    }
}

DWORD WINAPI GDI_One(LPVOID lpThread) {

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    int rw = screenW / 5;
    int rh = screenH / 5;

    HDC hdc = GetDC(NULL);
    HDC mem = CreateCompatibleDC(hdc);

    BITMAPINFO bi = { 0 };
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = rw;
    bi.bmiHeader.biHeight = -rh;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    RGBQUAD* px = NULL;
    HBITMAP bmp = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, (void**)&px, NULL, 0);

    if (!bmp)
        bmp = CreateCompatibleBitmap(hdc, rw, rh);

    SelectObject(mem, bmp);

    int t = 0;

    while (1) {

        StretchBlt(
            mem,
            0, 0, rw, rh,
            hdc,
            0, 0, screenW, screenH,
            SRCCOPY
        );

       
        for (int y = 0; y < rh; y++) {
            for (int x = 0; x < rw; x++) {

                int idx = y * rw + x;

             
                RGBQUAD old = px[idx];

               
                int v = ((x ^ y ^ t) & 255);

                RGBQUAD neo;
                neo.rgbRed = v;
                neo.rgbGreen = (BYTE)(v * 0.6f);
                neo.rgbBlue = 255 - v;

              
                float a = 0.35f;

                px[idx].rgbRed = (BYTE)(old.rgbRed * (1 - a) + neo.rgbRed * a);
                px[idx].rgbGreen = (BYTE)(old.rgbGreen * (1 - a) + neo.rgbGreen * a);
                px[idx].rgbBlue = (BYTE)(old.rgbBlue * (1 - a) + neo.rgbBlue * a);
            }
        }

     
        StretchBlt(
            hdc,
            0, 0, screenW, screenH,
            mem,
            0, 0, rw, rh,
            SRCCOPY
        );

        t++;
        Sleep(1);
    }

    return 0;
}

DWORD WINAPI GDI_Two(LPVOID lpThread) {
    const int width = GetSystemMetrics(0), height = GetSystemMetrics(1);

    HDC hdc = GetDC(NULL);
    HDC memdc = CreateCompatibleDC(hdc);

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;

    RGBQUAD* pixels;
    RGBQUAD* pixels_copy = (RGBQUAD*)malloc(width * height * 4);
    RGBQUAD* desktop = (RGBQUAD*)malloc(width * height * 4);

    HBITMAP hbit = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&pixels, 0, 0);
    SelectObject(memdc, hbit);

    int xc = width / 2;
    int yc = height / 2;

    int t = 0;
    int angle = 0;

    for (;;)
    {
        BitBlt(memdc, 0, 0, width, height, hdc, 0, 0, SRCCOPY);
        memcpy(pixels_copy, pixels, width * height * 4);
        memcpy(desktop, pixels, width * height * 4);

        int shift = (t * ShiftSpeed5) % width;
        t++;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int desty = yc + (x - xc) + sin(angle * (PAY / 150)) - (y - yc) * cos(angle * (PAY / 150));
                int destX = xc + (x - xc) * cos(angle * (PAY / 150)) + (y - yc) * sin(angle * (PAY / 150));

                int sx = x + shift;
                if (sx >= width) sx -= width;

                destX += width;
                destX %= width;
                desty += height;
                desty %= height;

                RGBQUAD base = desktop[y * width + x];

                float tcol = (float)sx / (float)width;

                BYTE r = (BYTE)(tcol * 255.0f);
                BYTE g = (BYTE)((1.0f - tcol) * 255.0f);
                BYTE b = (BYTE)((0.5f + 0.5f * tcol) * 255.0f);

                RGBQUAD out;
                out.rgbRed = base.rgbRed + r;
                out.rgbGreen = base.rgbGreen + g;
                out.rgbBlue = base.rgbBlue + b;
                out.rgbReserved = 255;

                pixels[y * width + x] = pixels_copy[desty * width + destX];
            }
        }

        angle++;

        BitBlt(hdc, 0, 0, width, height, memdc, 0, 0, SRCCOPY);
    }

    ReleaseDC(NULL, hdc);
    DeleteDC(memdc);
    DeleteObject(hbit);
    free(pixels_copy);
    free(desktop);

}


DWORD WINAPI GDI_Three(LPVOID lp) {
    int w = GetSystemMetrics(0);
    int h = GetSystemMetrics(1);

    HDC s = GetDC(NULL);
    HDC mem = CreateCompatibleDC(s);

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    RGBQUAD* px;
    RGBQUAD* cp = (RGBQUAD*)malloc(w * h * 4);

    HBITMAP bmp = CreateDIBSection(s, &bmi, DIB_RGB_COLORS, (void**)&px, NULL, 0);
    SelectObject(mem, bmp);

    int t = 0;
    int pix = 1;
    int pixCount = 0;

    for (;;) {
        BitBlt(mem, 0, 0, w, h, s, 0, 0, SRCCOPY);
        memcpy(cp, px, w * h * 4);

        for (int y = 0; y < h; y += pix) {
            for (int x = 0; x < w; x += pix) {
                int i = y * w + x;
                RGBQUAD c = cp[i];

                int r = c.rgbRed;
                int g = c.rgbGreen;
                int b = c.rgbBlue;

                int k = t * 4;
                int rr = (r + k * 3) & 255;
                int gg = (g + k * 5) & 255;
                int bb = (b + k * 7) & 255;

                for (int yy = 0; yy < pix; yy++) {
                    for (int xx = 0; xx < pix; xx++) {
                        int X = x + xx;
                        int Y = y + yy;
                        if (X < w && Y < h) {
                            int id = Y * w + X;
                            px[id].rgbRed = rr;
                            px[id].rgbGreen = gg;
                            px[id].rgbBlue = bb;
                        }
                    }
                }
            }
        }

        pixCount++;
        if (pixCount >= 10) {
            pixCount = 0;
            pix = 1;
            InvalidateRect(NULL, NULL, TRUE);
        }
        else {
            pix++;
        }

        BitBlt(s, 0, 0, w, h, mem, 0, 0, SRCCOPY);
        t++;
        Sleep(1);
    }

    return 0;
}

DWORD WINAPI GDI_Four(LPVOID lpThread) {
    HDC sdc = GetDC(0);
    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);

    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = sw;
    bi.bmiHeader.biHeight = -sh;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits;
    HBITMAP dib = CreateDIBSection(sdc, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
    HDC mdc = CreateCompatibleDC(sdc);
    SelectObject(mdc, dib);

    srand(GetTickCount());

    while (1) {
        BitBlt(mdc, 0, 0, sw, sh, sdc, 0, 0, SRCCOPY);

        RGBQUAD* px = (RGBQUAD*)bits;
        int count = sw * sh;
        for (int i = 0;i < count;i++) {
            BYTE r = px[i].rgbRed;
            BYTE g = px[i].rgbGreen;
            BYTE b = px[i].rgbBlue;
            px[i].rgbRed = (BYTE)((g + rand() % 64) & 0xFF);
            px[i].rgbGreen = (BYTE)((b + rand() % 64) & 0xFF);
            px[i].rgbBlue = (BYTE)((r + rand() % 64) & 0xFF);
        }

        int ox = (rand() % 21) - 10;
        int oy = (rand() % 21) - 10;
        BitBlt(sdc, ox, oy, sw - abs(ox), sh - abs(oy), mdc, 0, 0, SRCERASE);


        HBRUSH br = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
        HBRUSH old = (HBRUSH)SelectObject(sdc, br);
        SelectObject(sdc, old);
        DeleteObject(br);

        Sleep(1);
    }

    DeleteDC(mdc);
    DeleteObject(dib);
    ReleaseDC(0, sdc);
    return 0;
}


DWORD WINAPI GDI_Five(LPVOID lpThread) {
    HDC d = GetDC(0);
    int w = GetSystemMetrics(SM_CXSCREEN), h = GetSystemMetrics(SM_CYSCREEN);

    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.bmiHeader.biSize = 40;
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    RGBQUAD* p;
    HBITMAP bmp = CreateDIBSection(d, &bi, DIB_RGB_COLORS, (void**)&p, 0, 0);
    HDC m = CreateCompatibleDC(d);
    SelectObject(m, bmp);

    float t = 0.0f;

    for (;;) {
        for (int y = 0;y < h;y++) {
            for (int x = 0;x < w;x++) {
                float fx = (float)x;
                float fy = (float)y;
                float cx = w * 0.5f;
                float cy = h * 0.5f;
                float nx = (fx - cx) / 260.0f;
                float ny = (fy - cy) / 260.0f;
                float r2 = nx * nx + ny * ny;
                float d0 = sqrtf(r2);
                float ang = atan2f(ny, nx);

                float donutBand = fabsf(fmodf(d0 * 4.0f - t * 0.8f, 2.0f) - 1.0f);
                float donut = 1.0f - donutBand;
                donut *= smoothstep(0.25f, 0.55f, d0) * smoothstep(0.95f, 0.55f, d0);

                float ball = 1.0f - smoothstep(0.0f, 0.7f, d0);
                float ballShade = ball * (0.6f + 0.4f * (nx * 0.7f + ny * 0.4f + 0.3f));

                float plasmaBase =
                    sinf(fx * 0.025f + t * 0.9f) +
                    sinf(fy * 0.025f + t * 1.3f) +
                    sinf((fx + fy) * 0.018f + t * 0.7f);
                plasmaBase = plasmaBase * 0.166666f + 0.5f;

                float rainbowPhase = plasmaBase * 6.28318f + t * 0.7f;
                float pr = (sinf(rainbowPhase) * 0.5f + 0.5f);
                float pg = (sinf(rainbowPhase + 2.094f) * 0.5f + 0.5f);
                float pb = (sinf(rainbowPhase + 4.188f) * 0.5f + 0.5f);

                float shade = donut * 0.9f + ballShade * 1.1f + plasmaBase * 0.6f;
                if (shade < 0.0f) shade = 0.0f;
                if (shade > 1.0f) shade = 1.0f;

                float r = pr * shade;
                float g = pg * shade;
                float b = pb * shade;

                int idx = y * w + x;
                BYTE br = (BYTE)(b * 255.0f);
                BYTE bg = (BYTE)(g * 255.0f);
                BYTE bb = (BYTE)(r * 255.0f);

                p[idx].rgbRed = bb;
                p[idx].rgbGreen = bg;
                p[idx].rgbBlue = br;
                p[idx].rgbReserved = 0;
            }
        }

        int sq = 40;
        for (int y = 0;y < h;y++) {
            int ty = y / sq;
            for (int x = 0;x < w;x++) {
                int tx = x / sq;
                int idx = y * w + x;

                float hslh = fmodf((float)(tx + ty * 4) * 23.0f + t * 30.0f, 360.0f);
                float hsls = 1.0f;
                float hsll = 0.35f + 0.25f * sinf((tx * 0.7f + ty * 1.3f) + t * 0.9f);

                float C = (1.0f - fabsf(2.0f * hsll - 1.0f)) * hsls;
                float X = C * (1.0f - fabsf(fmodf(hslh / 60.0f, 2.0f) - 1.0f));
                float m0 = hsll - C * 0.5f;

                float rr, gg, bb;
                if (hslh < 60) { rr = C;gg = X;bb = 0; }
                else if (hslh < 120) { rr = X;gg = C;bb = 0; }
                else if (hslh < 180) { rr = 0;gg = C;bb = X; }
                else if (hslh < 240) { rr = 0;gg = X;bb = C; }
                else if (hslh < 300) { rr = X;gg = 0;bb = C; }
                else { rr = C;gg = 0;bb = X; }

                rr = (rr + m0);
                gg = (gg + m0);
                bb = (bb + m0);

                if (rr < 0) rr = 0; if (rr > 1) rr = 1;
                if (gg < 0) gg = 0; if (gg > 1) gg = 1;
                if (bb < 0) bb = 0; if (bb > 1) bb = 1;

                BYTE sr = (BYTE)(rr * 255.0f);
                BYTE sg = (BYTE)(gg * 255.0f);
                BYTE sb = (BYTE)(bb * 255.0f);

                BYTE dr = p[idx].rgbRed;
                BYTE dg = p[idx].rgbGreen;
                BYTE db = p[idx].rgbBlue;

                p[idx].rgbRed = (BYTE)((dr + sr) >> 1);
                p[idx].rgbGreen = (BYTE)((dg + sg) >> 1);
                p[idx].rgbBlue = (BYTE)((db + sb) >> 1);
            }
        }

        BitBlt(d, 0, 0, w, h, m, 0, 0, SRCCOPY);
        t += 0.02f;
        Sleep(10);
    }
    return 0;
}

DWORD WINAPI ShiftXW(LPVOID lpThread)
{
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);

    HBITMAP hbmTemp = CreateCompatibleBitmap(hdcScreen, w, h);
    SelectObject(hdcMem, hbmTemp);

    LARGE_INTEGER freq, t0, t1;
    QueryPerformanceFrequency(&freq);
    const double frameTime = 1.0 / 60.0;

    float speed = 0.0f;

    while (1)
    {
        QueryPerformanceCounter(&t0);

        BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, SRCCOPY);

        int shift = (int)speed;

        if (shift < w)
            BitBlt(hdcScreen, 0, 0, w, h, hdcMem, -speed, 0, SRCCOPY);
        BitBlt(hdcScreen, 0, 0, w, h, hdcMem, w - speed, 0, SRCCOPY);
        BitBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, -speed, SRCCOPY);
        BitBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, h - speed, SRCCOPY);

        speed += 0.1f;

        QueryPerformanceCounter(&t1);
        double elapsed = (double)(t1.QuadPart - t0.QuadPart) / (double)freq.QuadPart;

        while (elapsed < frameTime)
        {
            Sleep(0);
            QueryPerformanceCounter(&t1);
            elapsed = (double)(t1.QuadPart - t0.QuadPart) / (double)freq.QuadPart;
        }
    }
    


    return 0;
}

void shift_bw()
{
    HDC dc = GetDC(NULL);
    if (!dc) return;

    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);
    if (w <= 0 || h <= 0) { ReleaseDC(NULL, dc); return; }

    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    RGBQUAD* g_rgbScreen = NULL;
    HBITMAP bmp = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, (void**)&g_rgbScreen, NULL, 0);
    if (!bmp || !g_rgbScreen) {
        if (bmp) DeleteObject(bmp);
        ReleaseDC(NULL, dc);
        return;
    }

    HDC mdc = CreateCompatibleDC(dc);
    HGDIOBJ old = SelectObject(mdc, bmp);

    BitBlt(mdc, 0, 0, w, h, dc, 0, 0, SRCCOPY);

    int total = w * h;
    for (int i = 0; i < total; i++)
    {
        BYTE r = g_rgbScreen[i].rgbRed;
        BYTE g = g_rgbScreen[i].rgbGreen;
        BYTE b = g_rgbScreen[i].rgbBlue;

        int gray = (r + g + b) / 3;
        gray += 40;
        if (gray > 255) gray = 255;

        g_rgbScreen[i].rgbRed = (BYTE)gray;
        g_rgbScreen[i].rgbGreen = (BYTE)gray;
        g_rgbScreen[i].rgbBlue = (BYTE)gray;
        g_rgbScreen[i].rgbReserved = 0;
    }

    SetDIBitsToDevice(dc, 0, 0, w, h, 0, 0, 0, h, g_rgbScreen, &bi, DIB_RGB_COLORS);

    SelectObject(mdc, old);
    DeleteDC(mdc);
    DeleteObject(bmp);
    ReleaseDC(NULL, dc);
}



DWORD WINAPI GDI_Seven(LPVOID lpThread)
{
    HDC sdc = GetDC(NULL);
    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);

    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    RGBQUAD* buf;
    HBITMAP bmp = CreateDIBSection(sdc, &bi, DIB_RGB_COLORS, (void**)&buf, NULL, 0);
    HDC mdc = CreateCompatibleDC(sdc);
    SelectObject(mdc, bmp);

    float vLeft = 0.0f;
    float vRight = 0.0f;

    int half = w / 2;

    while (1)
    {
        BitBlt(mdc, 0, 0, w, h, sdc, 0, 0, SRCCOPY);

        vLeft += 2.0f;
        vRight -= 2.0f;

        if (vLeft >= w) vLeft -= w;
        if (vRight <= -w) vRight += w;

        BitBlt(sdc, 0, 0, half, h, mdc, (int)vLeft, 0, SRCCOPY);
        BitBlt(sdc, 0, 0, half, h, mdc, (int)(vLeft - w), 0, SRCCOPY);

        BitBlt(sdc, half, 0, half, h, mdc, (int)vRight, 0, SRCCOPY);
        BitBlt(sdc, half, 0, half, h, mdc, (int)(vRight + w), 0, SRCCOPY);

        Sleep(1);
    }

    return 0;
}


