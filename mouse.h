#ifndef MOUSE_H
#define MOUSE_H

#include <cstdint>

constexpr std::uint8_t BYTES_PER_REL = 3;
constexpr std::uint8_t BYTES_PER_ABS = 5;

struct Point {
    int x, y;
};

void myMouse(unsigned char* tempC[2], int buttonMask, Point curPos, Point oldPos, Point maxPos);

#endif // MOUSE_H
