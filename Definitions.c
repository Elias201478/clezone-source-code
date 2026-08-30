#include <Windows.h>
#include <math.h>
#include "Definitions.h"

float smoothstep(float a, float b, float x) {
    float t = (x - a) / (b - a);
    if (t < 0)t = 0;
    if (t > 1)t = 1;
    return t * t * (3 - 2 * t);
}


float hue2rgb(float p, float q, float t)
{
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;
    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
}


HSL rgb2hsl(RGBQUAD px) {
    HSL out;
    float R = px.rgbRed / 255.0f;
    float G = px.rgbGreen / 255.0f;
    float B = px.rgbBlue / 255.0f;

    float maxv = fmaxf(R, fmaxf(G, B));
    float minv = fminf(R, fminf(G, B));
    float delta = maxv - minv;

    out.l = (maxv + minv) * 0.5f;

    if (delta < 0.00001f) {
        out.h = 0.0f;
        out.s = 0.0f;
        return out;
    }

    out.s = (out.l < 0.5f)
        ? (delta / (maxv + minv))
        : (delta / (2.0f - maxv - minv));

    if (maxv == R)
        out.h = (G - B) / delta;
    else if (maxv == G)
        out.h = 2.0f + (B - R) / delta;
    else
        out.h = 4.0f + (R - G) / delta;

    out.h *= 60.0f;
    if (out.h < 0.0f) out.h += 360.0f;

    return out;
}

void HSLtoRGB(float H, float S, float L, int* r, int* g, int* b) {
    float R, G, B;

    H = fmodf(H, 360.0f);
    if (H < 0.0f) H += 360.0f;
    H /= 360.0f;

    if (S <= 0.0f) {
        R = G = B = L;
    }
    else {
        float q = (L < 0.5f) ? (L * (1.0f + S)) : (L + S - L * S);
        float p = 2.0f * L - q;

        R = hue2rgb(p, q, H + 1.0f / 3.0f);
        G = hue2rgb(p, q, H);
        B = hue2rgb(p, q, H - 1.0f / 3.0f);
    }

    *r = (int)(R * 255.0f);
    *g = (int)(G * 255.0f);
    *b = (int)(B * 255.0f);
}

RGBQUAD hsl2rgb(HSL hsl) {
    int r, g, b;
    HSLtoRGB(hsl.h, hsl.s, hsl.l, &r, &g, &b);

    RGBQUAD out;
    out.rgbRed = (BYTE)r;
    out.rgbGreen = (BYTE)g;
    out.rgbBlue = (BYTE)b;
    out.rgbReserved = 0;
    return out;
}

