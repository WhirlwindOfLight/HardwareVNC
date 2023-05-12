#ifndef WEBCAM_H
#define WEBCAM_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

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

private:
    int width;
    int height;
    std::vector<Pixel> pixels;
};

class Webcam {
public:
    Webcam(const std::string& dev, int resX, int resY, int fps);
    ~Webcam();

    Frame GetLastFrame();
    bool isNewFrame();

private:
    void Start();
    void Stop();
    void CaptureFrames();

    std::string device;
    int resolutionX;
    int resolutionY;
    int frameRate;
    bool isRunning;
    bool newFrameReady;
    Frame currentFrame;
    std::mutex frameMutex;
    std::thread captureThread;
};

#endif // WEBCAM_H
