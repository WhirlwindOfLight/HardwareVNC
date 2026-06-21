#include "webcam.h"

Frame::Frame(int width, int height) : width(width), height(height), pixels(width * height) {}

int Frame::getWidth() const {
    return width;
}

int Frame::getHeight() const {
    return height;
}

unsigned char* Frame::getRawData() {
    // Returns a pointer to the start of the underlying array
    return reinterpret_cast<unsigned char*>(this->pixels.data());
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

    {
        cv::Mat testFrame;
        capture.read(testFrame);

        if (testFrame.empty()) {
            // Stop attempting to capture frames and let the main thread handle things accordingly
            isRunning = false;
            capture.release();
            return;
        }

        if (testFrame.cols != resolutionX || testFrame.rows != resolutionY) {
            std::cerr << "[Warn] Camera capture rejected the requested resolution.\n"
                      << "| Requested: " << resolutionX << "x" << resolutionY << "\n"
                      << "| Received:  " << testFrame.cols << "x" << testFrame.rows << "\n";
        }
    }

    Frame frame(resolutionX, resolutionY);
    cv::Mat cvFrameWrap(
        resolutionY,
        resolutionX,
        CV_8UC3,
        frame.getRawData()
    );
    while (isRunning) {
        auto startTime = steady_clock::now();

        cv::Mat capturedFrame;
        capture.read(capturedFrame);
        if (capturedFrame.empty()) break;

        if (capturedFrame.cols != resolutionX || capturedFrame.rows != resolutionY) {
            cv::resize(capturedFrame, capturedFrame, cv::Size(resolutionX, resolutionY));
        }
        cv::cvtColor(capturedFrame, cvFrameWrap, cv::COLOR_BGR2RGB);

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
