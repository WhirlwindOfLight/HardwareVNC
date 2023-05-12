#pragma once

#include <cstdint>
#include <rfb/rfb.h>
#include <rfb/keysym.h>

constexpr std::uint8_t BYTES_PER_KEY = 2;
constexpr std::uint8_t KEY_SIG_BYTE = 5;

struct ModifierKeys {
    bool ctrl, shift, alt, super;
};

void myKeyboard(unsigned char msg[], unsigned char modKeyByte, unsigned char regKeyByte);
bool parseModKeys(ModifierKeys* modKeysPtr, rfbBool down, rfbKeySym key);
std::uint8_t modKeyToByte(ModifierKeys modKeys, bool needsShift);
bool needsShift(rfbKeySym key);
std::uint8_t keyToByte(rfbKeySym key);
