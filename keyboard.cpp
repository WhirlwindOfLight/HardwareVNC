#include <cstdint>
#include <rfb/rfb.h>
#include <rfb/keysym.h>
#include "keyboard.h"

void myKeyboard(std::uint8_t output[], std::uint8_t modKeyByte, std::uint8_t regKeyByte) {
    output[0] = BYTES_PER_KEY * 2 + 1;
    output[1] = output[2] = output[3] = 0;
    output[4] = KEY_SIG_BYTE;
    output[5] = modKeyByte;
    output[6] = regKeyByte;
    output[7] = modKeyByte;
    output[8] = 0;
}

// Returns true if a modKey was changed
bool parseModKeys(ModifierKeys* modKeysPtr, rfbBool down, rfbKeySym key) {
    switch (key) {
        case XK_Shift_L:
        case XK_Shift_R:
            modKeysPtr->shift = down;
            return true;
        case XK_Control_L:
        case XK_Control_R:
            modKeysPtr->ctrl = down;
            return true;
        case XK_Alt_L:
        case XK_Alt_R:
            modKeysPtr->alt = down;
            return true;
        case XK_Super_L:
        case XK_Super_R:
            modKeysPtr->super = down;
            return true;
        default:
            return false;
    }
}

std::uint8_t modKeyToByte(ModifierKeys modKeys, bool needsShift) {
    return modKeys.ctrl
        + (2 * (modKeys.shift || needsShift))
        + (4 * modKeys.alt)
        + (8 * modKeys.super);
}

bool needsShift(rfbKeySym key) {
    if (key >= 33 && key <= 126) {
        for (char i = 'A'; i <= 'Z'; i++) {
            if ((char)key == i) {
                return true;
            }
        }

        switch ((char)key) {
            case '~':
            case '!':
            case '@':
            case '#':
            case '$':
            case '%':
            case '^':
            case '&':
            case '*':
            case '(':
            case ')':
            case '_':
            case '+':
            case '{':
            case '}':
            case '|':
            case ':':
            case '\"':
            case '<':
            case '>':
            case '\?':
                return true;
            default:
                return false;
        }
    } else {
        return false;
    }
}

std::uint8_t keyToByte(rfbKeySym key) {
    if (key >= 33 && key <= 126) {
        // a-z + A-Z is 0x04-0x1D
        for (int i = 0; i < 26; i++) {
            if ((char)key == ('a' + i) || (char)key == ('A' + i)) {
                return i + 0x04;
            }
        }

        switch ((char)key) {
            case '1':
            case '!':
                return 0x1E;
            case '2':
            case '@':
                return 0x1F;
            case '3':
            case '#':
                return 0x20;
            case '4':
            case '$':
                return 0x21;
            case '5':
            case '%':
                return 0x22;
            case '6':
            case '^':
                return 0x23;
            case '7':
            case '&':
                return 0x24;
            case '8':
            case '*':
                return 0x25;
            case '9':
            case '(':
                return 0x26;
            case '0':
            case ')':
                return 0x27;
            case '-':
            case '_':
                return 0x2D;
            case '+':
            case '=':
                return 0x2E;
            case '[':
            case '{':
                return 0x2F;
            case ']':
            case '}':
                return 0x30;
            case '|':
            case '\\':
                return 0x31;
            case ':':
            case ';':
                return 0x33;
            case '\"':
            case '\'':
                return 0x34;
            case '`':
            case '~':
                return 0x35;
            case ',':
            case '<':
                return 0x36;
            case '.':
            case '>':
                return 0x37;
            case '/':
            case '\?':
                return 0x38;
            default:
                return 0x00;
        }
    } else {
        // F1-F12 is 0x3A-0x45
        for (int i = 0; i < 12; i++) {
            if (key == XK_F1 + i) {
                return i + 0x3A;
            }
        }

        switch (key) {
            case XK_Return:
                return 0x28;
            case XK_Escape:
                return 0x29;
            case XK_BackSpace:
                return 0x2A;
            case XK_Tab:
                return 0x2B;
            case XK_space:
                return 0x2C;
            case XK_Print:
                return 0x46;
            case XK_Pause:
                return 0x48;
            case XK_Insert:
                return 0x49;
            case XK_Home:
                return 0x4A;
            case XK_Page_Up:
                return 0x4B;
            case XK_Delete:
                return 0x4C;
            case XK_End:
                return 0x4D;
            case XK_Page_Down:
                return 0x4E;
            case XK_Right:
                return 0x4F;
            case XK_Left:
                return 0x50;
            case XK_Down:
                return 0x51;
            case XK_Up:
                return 0x52;
            default:
                return 0x00;
        }
    }
}

