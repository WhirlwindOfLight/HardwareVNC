#ifndef MOUSE_H
#define MOUSE_H

#include "stdDecls.h"

constexpr Byte BYTES_PER_REL = 3;
constexpr Byte BYTES_PER_ABS = 5;

struct Point {
    int x, y;
};

void myMouse(Byte* tempC[2], int buttonMask, Point curPos, Point oldPos, Point maxPos);

#endif // MOUSE_H
