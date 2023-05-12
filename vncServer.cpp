#include <iostream>
#include <rfb/rfb.h>
#include "webcam.h"
#include "keyboard.h"
#include "mouse.h"

#define CONTROLLER_PORT 19509
#define CONTROLLER_IP "10.1.2.1"
#define USE_CAMERA true
#define FPS 30
#define USE_DIFF_FRAMES false
int diffThreshold = 1;
int minContourArea = 100;
#define MOUSE_BOX_SIZE 50
#define LISTEN_PORT 5901
#define CAMERA_LOCATION "/dev/video0"
#define RFB_BPP 4
#define CAM_BPP 3
static const struct Point res = {1280, 720};
struct Point curPos = {};
rfbScreenInfoPtr rfbScreen;

#include <vector>
#include <opencv2/opencv.hpp>

#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define SA struct sockaddr
#include <arpa/inet.h>
#include <cstdlib>

int sockfd1, sockfd2, sockfd3;

struct Rect {
    Point topLeftCorner;
    Point bottomRightCorner;
};

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

static void initBuffer(unsigned char* buffer) {
    Pixel a = {0xff, 0x00, 0xff}; //magenta
    Pixel b = {0x00, 0x00, 0x00}; //black
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

int pixelDiff(Pixel* pixA, Pixel* pixB) {
    int diffR = abs(int(pixA->r) - int(pixB->r));
    int diffG = abs(int(pixA->g) - int(pixB->g));
    int diffB = abs(int(pixA->b) - int(pixB->b));
    return diffR + diffG + diffB;
}

std::vector<Rect> detectRectangles(const bool* boolArray, int rows, int cols, double minContourArea)
{
    // Create a binary matrix from the bool array
    cv::Mat binaryMatrix(rows, cols, CV_8U);
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            binaryMatrix.at<uchar>(i, j) = boolArray[i * cols + j] ? 255 : 0;
        }
    }

    std::vector<Rect> detectedRectangles;

    // Apply contour detection
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binaryMatrix, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Extract top-left and bottom-right corners of rectangles
    for (const auto& contour : contours)
    {
        // Calculate contour area
        double contourArea = cv::contourArea(contour);

        // Filter contours based on minimum area threshold
        if (contourArea >= minContourArea)
        {
            // Approximate contour to polygon
            std::vector<cv::Point> approx;
            cv::approxPolyDP(contour, approx, cv::arcLength(contour, true) * 0.02, true);

            // Filter and extract rectangles
            if (approx.size() == 4 && cv::isContourConvex(approx))
            {
                Point topLeft = { approx[0].x, approx[0].y };
                Point bottomRight = { approx[2].x, approx[2].y };

                detectedRectangles.push_back({ topLeft, bottomRight });
            }
        }
    }

    return detectedRectangles;
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
    Frame myFrame(res.x, res.y);
    if (USE_CAMERA) {
        w = new Webcam(CAMERA_LOCATION, res.x, res.y, FPS);

        while (true) {
            if (w->isNewFrame()) { 
                break;
            }
        }
        myFrame = w->GetLastFrame();
        cameraFrameToRfb((unsigned char*)rfbScreen->frameBuffer, &myFrame);
    }
    Frame oldFrame = myFrame;

    printf("Creating connections to controller on socket %s:%d...\n",CONTROLLER_IP, CONTROLLER_PORT);
    initSock(&sockfd1);
    initSock(&sockfd2);
    initSock(&sockfd3);
    printf("Connected to controller\n");

    rfbInitServer(rfbScreen);

    if (USE_CAMERA) {
        rfbMarkRectAsModified(rfbScreen, 0, 0, res.x, res.y);
        while(rfbIsActive(rfbScreen)) {
            if (w->isNewFrame()) {
            	//Capture the most recent frame
                myFrame = w->GetLastFrame();
                cameraFrameToRfb((unsigned char*)rfbScreen->frameBuffer, &myFrame);
                
                if (!USE_DIFF_FRAMES) {
                    rfbMarkRectAsModified(rfbScreen, 0, 0, res.x, res.y);
                } else {
                    //Draw the first box around the mouse
                    int x1 = curPos.x - (MOUSE_BOX_SIZE / 2); if(x1 < 0) x1 = 0;
                    int x2 = curPos.x + (MOUSE_BOX_SIZE / 2); if(x2 > res.x) x2 = res.x;
                    int y1 = curPos.y - (MOUSE_BOX_SIZE / 2); if(y1 < 0) y1 = 0;
                    int y2 = curPos.y + (MOUSE_BOX_SIZE / 2); if(y2 > res.y) y2 = res.y;
                    rfbMarkRectAsModified(rfbScreen, x1, y1, x2, y2);                

		    //Define an array of pixels that are different
		    bool* diffArray = new bool[res.x * res.y];
		    {
		        int i = 0;
                        for (int y = 0; y < res.y; y++) {
                            for (int x = 0; x < res.x; x++) {
                                diffArray[i] = (diffThreshold <= pixelDiff(&(myFrame.getPixel(x, y)), &(oldFrame.getPixel(x, y))));
                                i++;
                            }
                        }
                    }            

                    //Use the diffArray to generate rectangles
                    std::vector<Rect> detectedRectangles = detectRectangles(diffArray, res.y, res.x, minContourArea);
		    delete[] diffArray;
		
                    //Mark the rectangles as changes for the VNC server
                    for (const auto& rect : detectedRectangles) {
                        rfbMarkRectAsModified(rfbScreen, rect.topLeftCorner.x, rect.topLeftCorner.y, rect.bottomRightCorner.x, rect.bottomRightCorner.y);
                    }

                    //Set the old frame for the next loop
                    oldFrame = myFrame;
                }
                
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
