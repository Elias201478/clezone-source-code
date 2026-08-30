#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <windows.h>

typedef struct {
    float h;
    float s;
    float l;
} HSL;

float smoothstep(float a, float b, float x);
float hue2rgb(float p, float q, float t);
HSL rgb2hsl(RGBQUAD px);
RGBQUAD hsl2rgb(HSL hsl);
void HSLtoRGB(float H, float S, float L, int* r, int* g, int* b);

#endif
