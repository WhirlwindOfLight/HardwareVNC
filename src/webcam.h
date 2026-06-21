#ifndef WEBCAM_H
#define WEBCAM_H

#include <opencv2/opencv.hpp>
#include "stdDecls.h"

struct Pixel {
    unsigned char r, g, b;
};

class Frame {
public:
    Frame(int width, int height);

    int getWidth() const;
    int getHeight() const;

    Pixel& getPixel(int x, int y);
    const Pixel& getPixel(int x, int y) const;
    unsigned char* getRawData();

private:
    int width;
    int height;
    vector<Pixel> pixels;
};

class Webcam {
public:
    Webcam(const string& dev, int resX, int resY, int fps);
    ~Webcam();

    Frame GetLastFrame();
    bool isNewFrame();
    bool getIsRunning();

private:
    void Start();
    void Stop();
    void CaptureFrames();

    string device;
    int resolutionX;
    int resolutionY;
    int frameRate;
    bool isRunning;
    bool newFrameReady;
    Frame currentFrame;
    mutex frameMutex;
    thread captureThread;
};

#endif // WEBCAM_H
