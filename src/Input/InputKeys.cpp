// David Evans
// 2011/09/08
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "InputKeys.h"

namespace Improbability
{

MakeMetaConstant(unsigned, kMouseLeftButton,   VK_LBUTTON);
MakeMetaConstant(unsigned, kMouseRightButton,  VK_RBUTTON);
MakeMetaConstant(unsigned, kMouseMiddleButton, VK_MBUTTON);
MakeMetaConstant(unsigned, kMouseX1Button,     VK_XBUTTON1);
MakeMetaConstant(unsigned, kMouseX2Button,     VK_XBUTTON2);

MakeMetaConstant(unsigned, kKeyBackspace, VK_BACK);
MakeMetaConstant(unsigned, kKeyTab,       VK_TAB);
MakeMetaConstant(unsigned, kKeyClear,     VK_CLEAR);
MakeMetaConstant(unsigned, kKeyEnter,     VK_RETURN);
MakeMetaConstant(unsigned, kKeyShift,     VK_SHIFT);
MakeMetaConstant(unsigned, kKeyControl,   VK_CONTROL);
MakeMetaConstant(unsigned, kKeyAlt,       VK_MENU);
MakeMetaConstant(unsigned, kKeyCapsLock,  VK_CAPITAL);
MakeMetaConstant(unsigned, kKeyEscape,    VK_ESCAPE);
MakeMetaConstant(unsigned, kKeySpace,     VK_SPACE);
MakeMetaConstant(unsigned, kKeyPageUp,    VK_PRIOR);
MakeMetaConstant(unsigned, kKeyPageDown,  VK_NEXT);
MakeMetaConstant(unsigned, kKeyEnd,       VK_END);
MakeMetaConstant(unsigned, kKeyHome,      VK_HOME);
MakeMetaConstant(unsigned, kKeyLeft,      VK_LEFT);
MakeMetaConstant(unsigned, kKeyUp,        VK_UP);
MakeMetaConstant(unsigned, kKeyRight,     VK_RIGHT);
MakeMetaConstant(unsigned, kKeyDown,      VK_DOWN);
MakeMetaConstant(unsigned, kKeyInsert,    VK_INSERT);
MakeMetaConstant(unsigned, kKeyDelete,    VK_DELETE);
MakeMetaConstant(unsigned, kKey0,         '0');
MakeMetaConstant(unsigned, kKey1,         '1');
MakeMetaConstant(unsigned, kKey2,         '2');
MakeMetaConstant(unsigned, kKey3,         '3');
MakeMetaConstant(unsigned, kKey4,         '4');
MakeMetaConstant(unsigned, kKey5,         '5');
MakeMetaConstant(unsigned, kKey6,         '6');
MakeMetaConstant(unsigned, kKey7,         '7');
MakeMetaConstant(unsigned, kKey8,         '8');
MakeMetaConstant(unsigned, kKey9,         '9');
MakeMetaConstant(unsigned, kKeyA,         'A');
MakeMetaConstant(unsigned, kKeyB,         'B');
MakeMetaConstant(unsigned, kKeyC,         'C');
MakeMetaConstant(unsigned, kKeyD,         'D');
MakeMetaConstant(unsigned, kKeyE,         'E');
MakeMetaConstant(unsigned, kKeyF,         'F');
MakeMetaConstant(unsigned, kKeyG,         'G');
MakeMetaConstant(unsigned, kKeyH,         'H');
MakeMetaConstant(unsigned, kKeyI,         'I');
MakeMetaConstant(unsigned, kKeyJ,         'J');
MakeMetaConstant(unsigned, kKeyK,         'K');
MakeMetaConstant(unsigned, kKeyL,         'L');
MakeMetaConstant(unsigned, kKeyM,         'M');
MakeMetaConstant(unsigned, kKeyN,         'N');
MakeMetaConstant(unsigned, kKeyO,         'O');
MakeMetaConstant(unsigned, kKeyP,         'P');
MakeMetaConstant(unsigned, kKeyQ,         'Q');
MakeMetaConstant(unsigned, kKeyR,         'R');
MakeMetaConstant(unsigned, kKeyS,         'S');
MakeMetaConstant(unsigned, kKeyT,         'T');
MakeMetaConstant(unsigned, kKeyU,         'U');
MakeMetaConstant(unsigned, kKeyV,         'V');
MakeMetaConstant(unsigned, kKeyW,         'W');
MakeMetaConstant(unsigned, kKeyX,         'X');
MakeMetaConstant(unsigned, kKeyY,         'Y');
MakeMetaConstant(unsigned, kKeyZ,         'Z');
MakeMetaConstant(unsigned, kKeyLeftWin,   VK_LWIN);
MakeMetaConstant(unsigned, kKeyRightWin,  VK_RWIN);
MakeMetaConstant(unsigned, kKeyNumPad0,   VK_NUMPAD0);
MakeMetaConstant(unsigned, kKeyNumPad1,   VK_NUMPAD1);
MakeMetaConstant(unsigned, kKeyNumPad2,   VK_NUMPAD2);
MakeMetaConstant(unsigned, kKeyNumPad3,   VK_NUMPAD3);
MakeMetaConstant(unsigned, kKeyNumPad4,   VK_NUMPAD4);
MakeMetaConstant(unsigned, kKeyNumPad5,   VK_NUMPAD5);
MakeMetaConstant(unsigned, kKeyNumPad6,   VK_NUMPAD6);
MakeMetaConstant(unsigned, kKeyNumPad7,   VK_NUMPAD7);
MakeMetaConstant(unsigned, kKeyNumPad8,   VK_NUMPAD8);
MakeMetaConstant(unsigned, kKeyNumPad9,   VK_NUMPAD9);

MakeMetaConstant(unsigned, kKeyNumPadMultiply,  VK_MULTIPLY);
MakeMetaConstant(unsigned, kKeyNumPadAdd,       VK_ADD);
MakeMetaConstant(unsigned, kKeyNumPadSeperator, VK_SEPARATOR);
MakeMetaConstant(unsigned, kKeyNumPadSubtract,  VK_SUBTRACT);
MakeMetaConstant(unsigned, kKeyNumPadDecimal,   VK_DECIMAL);
MakeMetaConstant(unsigned, kKeyNumPadDivide,    VK_DIVIDE);
MakeMetaConstant(unsigned, kKeyF1,              VK_F1);
MakeMetaConstant(unsigned, kKeyF2,              VK_F2);
MakeMetaConstant(unsigned, kKeyF3,              VK_F3);
MakeMetaConstant(unsigned, kKeyF4,              VK_F4);
MakeMetaConstant(unsigned, kKeyF5,              VK_F5);
MakeMetaConstant(unsigned, kKeyF6,              VK_F6);
MakeMetaConstant(unsigned, kKeyF7,              VK_F7);
MakeMetaConstant(unsigned, kKeyF8,              VK_F8);
MakeMetaConstant(unsigned, kKeyF9,              VK_F9);
MakeMetaConstant(unsigned, kKeyF10,             VK_F10);
MakeMetaConstant(unsigned, kKeyF11,             VK_F11);
MakeMetaConstant(unsigned, kKeyF12,             VK_F12);
MakeMetaConstant(unsigned, kKeyNumLock,         VK_NUMLOCK);
MakeMetaConstant(unsigned, kKeyScrollLock,      VK_SCROLL);
MakeMetaConstant(unsigned, kKeyLeftShift,       VK_LSHIFT);
MakeMetaConstant(unsigned, kKeyRightShift,      VK_RSHIFT);
MakeMetaConstant(unsigned, kKeyLeftControl,     VK_LCONTROL);
MakeMetaConstant(unsigned, kKeyRightControl,    VK_RCONTROL);
MakeMetaConstant(unsigned, kKeyLeftAlt,         VK_LMENU);
MakeMetaConstant(unsigned, kKeyRightAlt,        VK_RMENU);

MakeMetaConstant(unsigned, kKeyMinus,           VK_OEM_MINUS);
MakeMetaConstant(unsigned, kKeyPlus,            VK_OEM_PLUS);
MakeMetaConstant(unsigned, kKeyComma,           VK_OEM_COMMA);
MakeMetaConstant(unsigned, kKeyPeriod,          VK_OEM_PERIOD);
MakeMetaConstant(unsigned, kKeyForwardSlash,    VK_OEM_2);
MakeMetaConstant(unsigned, kKeyTilde,           VK_OEM_3);
MakeMetaConstant(unsigned, kKeyLeftBracket,     VK_OEM_4);
MakeMetaConstant(unsigned, kKeyBackSlash,       VK_OEM_5);
MakeMetaConstant(unsigned, kKeyRightBracket,    VK_OEM_6);
MakeMetaConstant(unsigned, kKeyQuote,           VK_OEM_7);


}