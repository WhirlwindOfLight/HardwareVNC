#include "webcam.h"

Frame::Frame(int width, int height) : width(width), height(height), pixels(width * height) {}

int Frame::getWidth() const {
    return width;
}

int Frame::getHeight() const {
    return height;
}

Pixel& Frame::getPixel(int x, int y) {
    return pixels[y * width + x];
}

const Pixel& Frame::getPixel(int x, int y) const {
    return pixels[y * width + x];
}

Webcam::Webcam(const string& dev, int resX, int resY, int fps)
    : device(dev), resolutionX(resX), resolutionY(resY), frameRate(fps), isRunning(false), newFrameReady(false), currentFrame(resX, resY) {
    Start();
}

Webcam::~Webcam() {
    Stop();
}

Frame Webcam::GetLastFrame() {
    lock_guard<mutex> lock(frameMutex);
    return currentFrame;
}

bool Webcam::isNewFrame() {
    lock_guard<mutex> lock(frameMutex);
    return newFrameReady;
}

bool Webcam::getIsRunning() {
    return isRunning;
}

void Webcam::Start() {
    isRunning = true;
    captureThread = thread(&Webcam::CaptureFrames, this);
}

void Webcam::Stop() {
    isRunning = false;
    if (captureThread.joinable())
        captureThread.join();
}

void Webcam::CaptureFrames() {
    cv::VideoCapture capture(device);
    if (!capture.isOpened()) {
        cerr << "[Err] Failed to open camera: " << device << endl;
        isRunning = false;
        return;
    } else {
        cout << "Successfully opened camera: " << device << endl;
    }

    capture.set(cv::CAP_PROP_FRAME_WIDTH, resolutionX);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, resolutionY);

    Frame frame(resolutionX, resolutionY);
    while (isRunning) {
        auto startTime = steady_clock::now();

        cv::Mat cvFrame;
        capture.read(cvFrame);
        if (cvFrame.empty())
            break;

        for (int y = 0; y < resolutionY; ++y) {
            for (int x = 0; x < resolutionX; ++x) {
                cv::Vec3b bgr = cvFrame.at<cv::Vec3b>(y, x);
                frame.getPixel(x, y).r = bgr[2];
                frame.getPixel(x, y).g = bgr[1];
                frame.getPixel(x, y).b = bgr[0];
            }
        }

        {
            lock_guard<mutex> lock(frameMutex);
            newFrameReady = true;
            currentFrame = frame;
        }

        auto elapsedTime = steady_clock::now() - startTime;
        auto sleepDuration = milliseconds(1000 / frameRate) - elapsedTime;
        if (sleepDuration > milliseconds::zero())
            std::this_thread::sleep_for(sleepDuration);
    }

    cout << "[!] Camera capture thread loop ended" << endl;
    isRunning = false;
    capture.release();
}
