#define BYTES_PER_REL 3
#define BYTES_PER_ABS 5

struct point {
    int x,y;
};

void myMouse(unsigned char* tempC[2], int buttonMask, struct point curPos, struct point oldPos, struct point maxPos);
