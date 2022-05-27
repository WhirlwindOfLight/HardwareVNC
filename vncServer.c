#include <stdio.h>
#include <rfb/rfb.h>
#include "webcam.h"
#include "fps.h"
#include "keyboard.h"
#include "mouse.h"

#define CONTROLLER_PORT 19509
#define CONTROLLER_IP "10.1.2.1"
#define USE_CAMERA true
#define MOUSE_BOX_SIZE 50
#define LISTEN_PORT 5901
#define CAMERA_LOCATION "/dev/video0"
#define RFB_BPP 4
#define CAM_BPP 3
static const struct point res = {1280, 720};
struct point curPos = {};
rfbScreenInfoPtr rfbScreen;

#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define SA struct sockaddr

int sockfd1, sockfd2, sockfd3;

void initSock(int* sockfd) {
    struct sockaddr_in servaddr, cli;
   
    // socket create and verification
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        //printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(CONTROLLER_IP);
    servaddr.sin_port = htons(CONTROLLER_PORT);
   
    // connect the client socket to server socket
    if (connect(*sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("a connection with the controller server failed...\n");
        exit(0);
    }
    //else
        //printf("connected to the server..\n");
   
}

struct color {
    unsigned char r,g,b;
};

static void initBuffer(unsigned char* buffer) {
    struct color a = {0xff, 0x00, 0xff}; //magenta
    struct color b = {0x00, 0x00, 0x00}; //black
    for (int row = 0; row < res.y; row++) {
        for (int col = 0; col < res.x; col++) {
            bool pos1 = row / 100 % 2 && !(col / 100 % 2);
            bool pos2 = !(row / 100 % 2) && col / 100 % 2;
            if (pos1 || pos2) {
                buffer[(row*res.x+col)*RFB_BPP+0] = a.r;
                buffer[(row*res.x+col)*RFB_BPP+1] = a.g;
                buffer[(row*res.x+col)*RFB_BPP+2] = a.b;
                buffer[(row*res.x+col)*RFB_BPP+3] = 0;
            } else {
                buffer[(row*res.x+col)*RFB_BPP+0] = b.r;
                buffer[(row*res.x+col)*RFB_BPP+1] = b.g;
                buffer[(row*res.x+col)*RFB_BPP+2] = b.b;
                buffer[(row*res.x+col)*RFB_BPP+3] = 0;
            }
        }
    }
}

static void cameraFrameToRfb(unsigned char* rfbBuffer, buffer_t* cameraFrame) {
    for (int row = 0; row < res.y; row++) {
        for (int col = 0; col < res.x; col++) {
            if ((row*res.x+col)*CAM_BPP+2 < cameraFrame->length) {
                rfbBuffer[(row*res.x+col)*RFB_BPP+0] = cameraFrame->start[(row*res.x+col)*CAM_BPP+0];
                rfbBuffer[(row*res.x+col)*RFB_BPP+1] = cameraFrame->start[(row*res.x+col)*CAM_BPP+1];
                rfbBuffer[(row*res.x+col)*RFB_BPP+2] = cameraFrame->start[(row*res.x+col)*CAM_BPP+2];
                rfbBuffer[(row*res.x+col)*RFB_BPP+3] = 0;
            }
        }
    }
}

static void dokey(rfbBool down,rfbKeySym key,rfbClientPtr cl) {
    static struct modifierKeys modKeys = {};
    unsigned char regByte = keyToByte(key);
    if (parseModKeys(&modKeys, down, key) || (down && regByte != 0x00)) {
        unsigned char output[BYTES_PER_KEY * 2 + 5];
        unsigned char modByte = modKeyToByte(modKeys, needsShift(key));
        myKeyboard(output, modByte, regByte);

        write(sockfd3, output, sizeof(output));

        /*
        printf("\n");
        for (int i = 0; i < BYTES_PER_KEY * 2 + 5; i++) {
            printf("0x%x ", output[i]);
        }
        printf("\n");
        */
    }
    if (!USE_CAMERA) rfbProcessEvents(rfbScreen,rfbScreen->deferUpdateTime*1000);

}

static void doptr(int buttonMask,int x,int y,rfbClientPtr cl)
{
    static struct point oldPos = {};
    curPos.x = x;
    curPos.y = y;

    unsigned char tempA[BYTES_PER_REL * 2 + 5];
    unsigned char tempB[BYTES_PER_ABS * 2 + 5];
    unsigned char* output[] = {tempA, tempB};

    myMouse(output, buttonMask, curPos, oldPos, res);
    write(sockfd1, tempA, sizeof(tempA));
    write(sockfd2, tempB, sizeof(tempB));

    oldPos = curPos;
    if (!USE_CAMERA) rfbProcessEvents(rfbScreen,rfbScreen->deferUpdateTime*1000);
}

int main(int argc, char** argv) {
    rfbScreen = rfbGetScreen(&argc, argv, res.x, res.y, 8, 3, RFB_BPP);
    if (!rfbScreen)
        return 1;
    rfbScreen->desktopName = "This is a test";
    rfbScreen->frameBuffer = (char*)malloc(res.x*res.y*RFB_BPP);
    rfbScreen->ptrAddEvent = doptr;
    rfbScreen->kbdAddEvent = dokey;
    rfbScreen->port = LISTEN_PORT;
    rfbScreen->ipv6port = LISTEN_PORT;


    initBuffer((unsigned char*)rfbScreen->frameBuffer);
    rfbMarkRectAsModified(rfbScreen, 0, 0, res.x, res.y);
    webcam_t* w;
    buffer_t frame;
    if (USE_CAMERA) {
        w = webcam_open(CAMERA_LOCATION);
        webcam_resize(w, res.x, res.y);
        webcam_stream(w, true);

        while (true) {
            frame = webcam_grab(w);
            if (frame.length > 0) { 
                break;
            }
            free(frame.start);
        }
        cameraFrameToRfb((unsigned char*)rfbScreen->frameBuffer, &frame);
    }

    printf("Creating connections to controller on socket %s:%d...\n",CONTROLLER_IP, CONTROLLER_PORT);
    initSock(&sockfd1);
    initSock(&sockfd2);
    initSock(&sockfd3);
    printf("Connected to controller\n");

    rfbInitServer(rfbScreen);

    if (USE_CAMERA) {
        int camUpsPerSect = 4;
        int rfbFps = 15;
        double sectSize = 16.0;
        int fNum = 1;
        int sectNum = 1;
        while(rfbIsActive(rfbScreen)) {

            if (TimeToTakePicture(camUpsPerSect * rfbFps * sectSize)) {
                frame = webcam_grab(w);
                cameraFrameToRfb((unsigned char*)rfbScreen->frameBuffer, &frame);
                free(frame.start);
                
                if (fNum == camUpsPerSect) {
                    //rfbMarkRectAsModified(rfbScreen, 0, 0, resX, resY);            
                    rfbMarkRectAsModified(rfbScreen, 0, (sectNum - 1) * (res.y / sectSize), res.x, sectNum * (res.y / sectSize));
                    if (sectNum == sectSize) {
                        sectNum = 1;
                    } else {
                        sectNum++;
                    }
                    fNum = 1;
                } else {
                    fNum++;
                }

                int x1 = curPos.x - (MOUSE_BOX_SIZE / 2); if(x1 < 0) x1 = 0;
                int x2 = curPos.x + (MOUSE_BOX_SIZE / 2); if(x2 > res.x) x2 = res.x;
                int y1 = curPos.y - (MOUSE_BOX_SIZE / 2); if(y1 < 0) y1 = 0;
                int y2 = curPos.y + (MOUSE_BOX_SIZE / 2); if(y2 > res.y) y2 = res.y;
            
                rfbMarkRectAsModified(rfbScreen, x1, y1, x2, y2);
                
                rfbProcessEvents(rfbScreen,rfbScreen->deferUpdateTime*1000);
            }
        }
    } else {
        rfbRunEventLoop(rfbScreen,40000,FALSE);
    }

    close(sockfd1);
    close(sockfd2);
    close(sockfd3);

    if (USE_CAMERA) {
        webcam_stream(w, false);
        webcam_close(w);
        free(frame.start);
    }
    free(rfbScreen->frameBuffer);
    rfbScreenCleanup(rfbScreen);

    return(0);
}
