#include <iostream>
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
static const struct Point res = {1280, 720};
struct Point curPos = {};
rfbScreenInfoPtr rfbScreen;

#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define SA struct sockaddr
#include <arpa/inet.h>

int sockfd1, sockfd2, sockfd3;

void initSock(int* sockfd) {
    struct sockaddr_in servaddr, cli;
   
    // socket create and verification
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sockfd == -1) {
        std::cout << "socket creation failed..." << std::endl;
        exit(0);
    }
    else
        //std::cout << "Socket successfully created.." << std::endl;
    bzero(&servaddr, sizeof(servaddr));
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(CONTROLLER_IP);
    servaddr.sin_port = htons(CONTROLLER_PORT);
   
    // connect the client socket to server socket
    if (connect(*sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        std::cout << "a connection with the controller server failed..." << std::endl;
        exit(0);
    }
    //else
        //std::cout << "connected to the server.." << std::endl;
   
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

static void cameraFrameToRfb(unsigned char* rfbBuffer, Frame* cameraFrame) {
    for (int row = 0; row < res.y; row++) {
        for (int col = 0; col < res.x; col++) {
            if ((row*res.x+col)*CAM_BPP+2 < (cameraFrame->getWidth() * cameraFrame->getHeight() * CAM_BPP)) {
                rfbBuffer[(row*res.x+col)*RFB_BPP+0] = cameraFrame->getPixel(col, row).r;
                rfbBuffer[(row*res.x+col)*RFB_BPP+1] = cameraFrame->getPixel(col, row).g;
                rfbBuffer[(row*res.x+col)*RFB_BPP+2] = cameraFrame->getPixel(col, row).b;
                rfbBuffer[(row*res.x+col)*RFB_BPP+3] = 0;
            }
        }
    }
}

static void dokey(rfbBool down,rfbKeySym key,rfbClientPtr cl) {
    static struct ModifierKeys modKeys = {};
    unsigned char regByte = keyToByte(key);
    if (parseModKeys(&modKeys, down, key) || (down && regByte != 0x00)) {
        unsigned char output[BYTES_PER_KEY * 2 + 5];
        unsigned char modByte = modKeyToByte(modKeys, needsShift(key));
        myKeyboard(output, modByte, regByte);

        write(sockfd3, output, sizeof(output));

        /*
        std::cout << std::endl;
        for (int i = 0; i < BYTES_PER_KEY * 2 + 5; i++) {
            std::cout << "0x" << std::hex << (int)output[i] << " ";
        }
        std::cout << std::endl;
        */
    }
    if (!USE_CAMERA) rfbProcessEvents(rfbScreen,rfbScreen->deferUpdateTime*1000);

}

static void doptr(int buttonMask,int x,int y,rfbClientPtr cl)
{
    static struct Point oldPos = {};
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
    Webcam* w;
    //buffer_t frame;
    Frame frame(res.x, res.y);
    if (USE_CAMERA) {
        w = new Webcam(CAMERA_LOCATION, res.x, res.y, 60);

        while (true) {
            if (w->isNewFrame()) { 
                break;
            }
        }
        frame = w->GetLastFrame();
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

            if (true || TimeToTakePicture(camUpsPerSect * rfbFps * sectSize)) {
                frame = w->GetLastFrame();
                cameraFrameToRfb((unsigned char*)rfbScreen->frameBuffer, &frame);
                
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
    	delete w;
    }
    free(rfbScreen->frameBuffer);
    rfbScreenCleanup(rfbScreen);

    return 0;
}
