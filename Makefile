CXX = clang++
CXXFLAGS = -std=c++11 -I/usr/include/opencv4
LDFLAGS = -L/usr/lib -lopencv_core -lvncserver -lopencv_videoio
EXECUTABLE = hardwareVncServer

all: $(EXECUTABLE)

$(EXECUTABLE): vncServer.cpp keyboard.cpp mouse.cpp webcam.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(EXECUTABLE)
