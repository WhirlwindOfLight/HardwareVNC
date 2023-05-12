CXX = clang++
CXXFLAGS = -std=c++11 -I/usr/include/opencv4
LDFLAGS = -L/usr/lib -lvncserver -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio
EXECUTABLE = hardwareVncServer

all: $(EXECUTABLE)

$(EXECUTABLE): vncServer.cpp keyboard.cpp mouse.cpp webcam.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(EXECUTABLE)
