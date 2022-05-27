#include <stdio.h>
#include <stdbool.h>
#include "mouse.h"
#define BYTES_PER_REL 3
#define BYTES_PER_ABS 5
#define REL_SIG_BYTE 6
#define ABS_SIG_BYTE 7
#define W_PER_TICK 10
#define USE_REL_MOUSE false

struct mouseButton {
    bool left;
    bool middle;
    bool right;
    bool wheelUp;
    bool wheelDown;
};

struct absMouseBytes {
    unsigned char bigX;
    unsigned char littleX;
    unsigned char bigY;
    unsigned char littleY;
    unsigned char wheel;
};

struct relMouseBytes {
    unsigned char button;
    unsigned char x;
    unsigned char y;
};

int toBitPercent(int in, int max) {
    return (double)in / max * 32767;
}

struct mouseButton btnMaskToStruct(int btns) {
    struct mouseButton onBtns = {};
    if (onBtns.wheelDown = btns >= 16) {
        btns -= 16;
    }
    if (onBtns.wheelUp = btns >= 8) {
        btns -= 8;
    }
    if (onBtns.right = btns >= 4) {
        btns -= 4;
    }
    if (onBtns.middle = btns >= 2) {
        btns -= 2;
    }
    onBtns.left = btns;
    return onBtns;
}

struct absMouseBytes initAbsMouseBytes(struct mouseButton onBtns, struct point myPoint, struct point max, bool calcPoint) {
    struct absMouseBytes temp = {};
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

struct relMouseBytes initRelMouseBytes(struct mouseButton onBtns, struct point curPos, struct point oldPos, bool calcPoint) {
    struct relMouseBytes temp = {};
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

void myMouse(unsigned char* output[2], int btns, struct point curPos, struct point oldPos, struct point maxPos) {
    struct mouseButton onBtns = btnMaskToStruct(btns);
    struct relMouseBytes rel = initRelMouseBytes(onBtns, curPos, oldPos, USE_REL_MOUSE);
    struct absMouseBytes abs = initAbsMouseBytes(onBtns, curPos, maxPos, !USE_REL_MOUSE);

    output[0][0] = BYTES_PER_REL * 2 + 1;
    output[0][1] = output[0][2] = output [0][3] = 0;
    output[0][4] = REL_SIG_BYTE;
    output[0][5] = rel.button;
    output[0][6] = rel.x;
    output[0][7] = rel.y;
    output[0][8] = rel.button;
    output[0][9] = output[0][10] = 0;

    output[1][0] = BYTES_PER_ABS * 2 + 1;
    output[1][1] = output [1][2] = output[1][3] = 0;
    output[1][4] = ABS_SIG_BYTE;
    output[1][5] = abs.littleX;
    output[1][6] = abs.bigX;
    output[1][7] = abs.littleY;
    output[1][8] = abs.bigY;
    output[1][9] = abs.wheel; 
    output[1][10] = output[1][11] = output[1][12] = output[1][13] = 0;
    output[1][14] = abs.wheel;
    //tempC[0] = tempA;
    //tempC[1] = tempB;

    /*
    printf("\n");
    for (int i = 0; i < BYTES_PER_REL * 2 + 5; i++) {
        printf("0x%x ", tempC[0][i]);
    }
    printf("\n");
    */
}
