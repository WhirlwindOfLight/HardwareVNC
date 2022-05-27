#include <stdbool.h>
#include <rfb/rfb.h>
#include <rfb/keysym.h>
#define BYTES_PER_KEY 2
#define KEY_SIG_BYTE 5

struct modifierKeys {
    bool ctrl, shift, alt, super;
};

void myKeyboard(unsigned char msg[], unsigned char modKeyByte, unsigned char regKeyByte);
bool parseModKeys(struct modifierKeys* modKeysPtr, rfbBool down, rfbKeySym key);
unsigned char modKeyToByte(struct modifierKeys modKeys, bool needsShift);
bool needsShift(rfbKeySym key);
unsigned char keyToByte(rfbKeySym key);
