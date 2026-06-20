cmake_minimum_required(VERSION 3.15)
project(HardwareVNC VERSION 1.0.0 LANGUAGES CXX)

set(BIN_NAME "hardware-vnc")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenCV REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(VNC REQUIRED IMPORTED_TARGET libvncserver)

file(GLOB SOURCES "src/*.cpp")
add_executable(${BIN_NAME} ${SOURCES})
target_link_libraries(${BIN_NAME} 
    PRIVATE 
    ${OpenCV_LIBS}
    yaml-cpp::yaml-cpp
    PkgConfig::VNC
)

install(TARGETS ${BIN_NAME} DESTINATION bin)