#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <filesystem>
#include <cstdlib>

using std::cout;
using std::cerr;
using std::endl;
using std::hex;

using std::runtime_error;
using std::exception;

using std::string;
using std::unordered_map;
using std::vector;
using Byte = std::uint8_t;

using std::chrono::steady_clock;
using std::chrono::milliseconds;

using std::mutex;
using std::lock_guard;
using std::thread;

namespace fs = std::filesystem;