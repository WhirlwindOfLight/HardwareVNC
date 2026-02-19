#pragma once

#include <rfb/rfb.h>
#include <rfb/keysym.h>
#include "stdDecls.h"

constexpr Byte BYTES_PER_KEY = 2;
constexpr Byte KEY_SIG_BYTE = 5;

struct ModifierKeys {
    bool ctrl, shift, alt, super;
};

void myKeyboard(unsigned char msg[], unsigned char modKeyByte, unsigned char regKeyByte);
bool parseModKeys(ModifierKeys* modKeysPtr, rfbBool down, rfbKeySym key);
Byte modKeyToByte(ModifierKeys modKeys, bool needsShift);
bool needsShift(rfbKeySym key);
Byte keyToByte(rfbKeySym key);
