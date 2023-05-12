#include <iostream>
#include "mouse.h"

//constexpr std::uint8_t BYTES_PER_REL = 3;
//constexpr std::uint8_t BYTES_PER_ABS = 5;
constexpr std::uint8_t REL_SIG_BYTE = 6;
constexpr std::uint8_t ABS_SIG_BYTE = 7;
constexpr int W_PER_TICK = 10;
constexpr bool USE_REL_MOUSE = false;

struct MouseButton {
    bool left;
    bool middle;
    bool right;
    bool wheelUp;
    bool wheelDown;
};

struct AbsMouseBytes {
    std::uint8_t bigX;
    std::uint8_t littleX;
    std::uint8_t bigY;
    std::uint8_t littleY;
    std::uint8_t wheel;
};

struct RelMouseBytes {
    std::uint8_t button;
    std::uint8_t x;
    std::uint8_t y;
};

int toBitPercent(int in, int max) {
    return static_cast<double>(in) / max * 32767;
}

MouseButton btnMaskToStruct(int btns) {
    MouseButton onBtns = {};

    onBtns.wheelDown = (btns >= 16);
    if (onBtns.wheelDown) {
        btns -= 16;
    }

    onBtns.wheelUp = (btns >= 8);
    if (onBtns.wheelUp) {
        btns -= 8;
    }

    onBtns.right = (btns >= 4);
    if (onBtns.right) {
        btns -= 4;
    }

    onBtns.middle = (btns >= 2);
    if (onBtns.middle) {
        btns -= 2;
    }
    onBtns.left = btns;
    return onBtns;
}

AbsMouseBytes initAbsMouseBytes(MouseButton onBtns, Point myPoint, Point max, bool calcPoint) {
    AbsMouseBytes temp = {};
    if (calcPoint) {
        temp.bigX = (toBitPercent(myPoint.x, max.x) / 256);
        temp.littleX = (toBitPercent(myPoint.x, max.x) % 256);
        temp.bigY = (toBitPercent(myPoint.y, max.y) / 256);
        temp.littleY = (toBitPercent(myPoint.y, max.y) % 256);
    } else {
        temp.bigX = 0;
        temp.littleX = 0;
        temp.bigY = 0;
        temp.littleY = 0;
    }
    if (onBtns.wheelUp) {
        temp.wheel = -W_PER_TICK;
    } else if (onBtns.wheelDown) {
        temp.wheel = W_PER_TICK;
    } else {
        temp.wheel = 0;
    }
    return temp;
}

RelMouseBytes initRelMouseBytes(MouseButton onBtns, Point curPos, Point oldPos, bool calcPoint) {
    RelMouseBytes temp = {};
    temp.button = onBtns.left * 1 + onBtns.right * 2 + onBtns.middle * 4;
    if (calcPoint) {
        temp.x = curPos.x - oldPos.x;
        temp.y = curPos.y - oldPos.y;
    } else {
        temp.x = 0;
        temp.y = 0;
    }
    return temp;
}

void myMouse(unsigned char* output[2], int btns, Point curPos, Point oldPos, Point maxPos) {
    MouseButton onBtns = btnMaskToStruct(btns);
    RelMouseBytes rel = initRelMouseBytes(onBtns, curPos, oldPos, USE_REL_MOUSE);
    AbsMouseBytes abs = initAbsMouseBytes(onBtns, curPos, maxPos, !USE_REL_MOUSE);

    output[0][0] = BYTES_PER_REL * 2 + 1;
    output[0][1] = output[0][2] = output[0][3] = 0;
    output[0][4] = REL_SIG_BYTE;
    output[0][5] = rel.button;
    output[0][6] = rel.x;
    output[0][7] = rel.y;
    output[0][8] = rel.button;
    output[0][9] = output[0][10] = 0;

    output[1][0] = BYTES_PER_ABS * 2 + 1;
    output[1][1] = output[1][2] = output[1][3] = 0;
    output[1][4] = ABS_SIG_BYTE;
    output[1][5] = abs.littleX;
    output[1][6] = abs.bigX;
    output[1][7] = abs.littleY;
    output[1][8] = abs.bigY;
    output[1][9] = abs.wheel; 
    output[1][10] = output[1][11] = output[1][12] = output[1][13] = 0;
    output[1][14] = abs.wheel;

    // tempC[0] = tempA;
    // tempC[1] = tempB;

    /*
    std::cout << std::endl;
    for (int i = 0; i < BYTES_PER_REL * 2 + 5; i++) {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(tempC[0][i]) << " ";
    }
    std::cout << std::endl;
    */
}
