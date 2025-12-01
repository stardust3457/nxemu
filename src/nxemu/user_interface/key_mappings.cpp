#include "key_mappings.h"
#include <unordered_map>
#include <sciter_handler.h>
#include <yuzu_common/settings_input.h>

enum VirtualKeyCodes
{
    VK_BACK = 0x08,
    VK_TAB = 0x09,
    VK_RETURN = 0x0D,
    VK_SHIFT = 0x10,
    VK_CONTROL = 0x11,
    VK_MENU = 0x12,
    VK_PAUSE = 0x13,
    VK_CAPITAL = 0x14,
    VK_ESCAPE = 0x1B,
    VK_SPACE = 0x20,
    VK_PRIOR = 0x21,
    VK_NEXT = 0x22,
    VK_END = 0x23,
    VK_HOME = 0x24,
    VK_LEFT = 0x25,
    VK_UP = 0x26,
    VK_RIGHT = 0x27,
    VK_DOWN = 0x28,
    VK_INSERT = 0x2D,
    VK_DELETE = 0x2E,
    VK_NUMPAD0 = 0x60,
    VK_NUMPAD1 = 0x61,
    VK_NUMPAD2 = 0x62,
    VK_NUMPAD3 = 0x63,
    VK_NUMPAD4 = 0x64,
    VK_NUMPAD5 = 0x65,
    VK_NUMPAD6 = 0x66,
    VK_NUMPAD7 = 0x67,
    VK_NUMPAD8 = 0x68,
    VK_NUMPAD9 = 0x69,
    VK_MULTIPLY = 0x6A,
    VK_ADD = 0x6B,
    VK_SUBTRACT = 0x6D,
    VK_DECIMAL = 0x6E,
    VK_DIVIDE = 0x6F,
    VK_F1 = 0x70,
    VK_F2 = 0x71,
    VK_F3 = 0x72,
    VK_F4 = 0x73,
    VK_F5 = 0x74,
    VK_F6 = 0x75,
    VK_F7 = 0x76,
    VK_F8 = 0x77,
    VK_F9 = 0x78,
    VK_F10 = 0x79,
    VK_F11 = 0x7A,
    VK_F12 = 0x7B,
    VK_F13 = 0x7C,
    VK_F14 = 0x7D,
    VK_F15 = 0x7E,
    VK_F16 = 0x7F,
    VK_F17 = 0x80,
    VK_F18 = 0x81,
    VK_F19 = 0x82,
    VK_F20 = 0x83,
    VK_F21 = 0x84,
    VK_F22 = 0x85,
    VK_F23 = 0x86,
    VK_F24 = 0x87,
    VK_OEM_1 = 0xBA,      // ';:' for US
    VK_OEM_PLUS = 0xBB,   // '+' any country
    VK_OEM_COMMA = 0xBC,  // ',' any country
    VK_OEM_MINUS = 0xBD,  // '-' any country
    VK_OEM_PERIOD = 0xBE, // '.' any country
    VK_OEM_2 = 0xBF,      // '/?' for US
    VK_OEM_3 = 0xC0,      // '`~' for US
    VK_OEM_4 = 0xDB,      // '[{' for US
    VK_OEM_5 = 0xDC,      // '\|' for US
    VK_OEM_6 = 0xDD,      // ']}' for US
    VK_OEM_7 = 0xDE       // ''"' for US
};

int32_t SciterKeyToSwitchKey(SciterKeys key)
{
    static const std::unordered_map<uint32_t, int32_t> keyMappings = {
        {'A', InputSettings::NativeKeyboard::A},
        {'B', InputSettings::NativeKeyboard::B},
        {'C', InputSettings::NativeKeyboard::C},
        {'D', InputSettings::NativeKeyboard::D},
        {'E', InputSettings::NativeKeyboard::E},
        {'F', InputSettings::NativeKeyboard::F},
        {'G', InputSettings::NativeKeyboard::G},
        {'H', InputSettings::NativeKeyboard::H},
        {'I', InputSettings::NativeKeyboard::I},
        {'J', InputSettings::NativeKeyboard::J},
        {'K', InputSettings::NativeKeyboard::K},
        {'L', InputSettings::NativeKeyboard::L},
        {'M', InputSettings::NativeKeyboard::M},
        {'N', InputSettings::NativeKeyboard::N},
        {'O', InputSettings::NativeKeyboard::O},
        {'P', InputSettings::NativeKeyboard::P},
        {'Q', InputSettings::NativeKeyboard::Q},
        {'R', InputSettings::NativeKeyboard::R},
        {'S', InputSettings::NativeKeyboard::S},
        {'T', InputSettings::NativeKeyboard::T},
        {'U', InputSettings::NativeKeyboard::U},
        {'V', InputSettings::NativeKeyboard::V},
        {'W', InputSettings::NativeKeyboard::W},
        {'X', InputSettings::NativeKeyboard::X},
        {'Y', InputSettings::NativeKeyboard::Y},
        {'Z', InputSettings::NativeKeyboard::Z},
        {'1', InputSettings::NativeKeyboard::N1},
        {'2', InputSettings::NativeKeyboard::N2},
        {'3', InputSettings::NativeKeyboard::N3},
        {'4', InputSettings::NativeKeyboard::N4},
        {'5', InputSettings::NativeKeyboard::N5},
        {'6', InputSettings::NativeKeyboard::N6},
        {'7', InputSettings::NativeKeyboard::N7},
        {'8', InputSettings::NativeKeyboard::N8},
        {'9', InputSettings::NativeKeyboard::N9},
        {'0', InputSettings::NativeKeyboard::N0},
        {SCITER_KEY_ENTER, InputSettings::NativeKeyboard::Return},
        {SCITER_KEY_ESCAPE, InputSettings::NativeKeyboard::Escape},
        {SCITER_KEY_TAB, InputSettings::NativeKeyboard::Tab},
        {SCITER_KEY_SPACE, InputSettings::NativeKeyboard::Space},
        {SCITER_KEY_BACKSPACE, InputSettings::NativeKeyboard::Backspace},
        {SCITER_KEY_F1, InputSettings::NativeKeyboard::F1},
        {SCITER_KEY_F2, InputSettings::NativeKeyboard::F2},
        {SCITER_KEY_F3, InputSettings::NativeKeyboard::F3},
        {SCITER_KEY_F4, InputSettings::NativeKeyboard::F4},
        {SCITER_KEY_F5, InputSettings::NativeKeyboard::F5},
        {SCITER_KEY_F6, InputSettings::NativeKeyboard::F6},
        {SCITER_KEY_F7, InputSettings::NativeKeyboard::F7},
        {SCITER_KEY_F8, InputSettings::NativeKeyboard::F8},
        {SCITER_KEY_F9, InputSettings::NativeKeyboard::F9},
        {SCITER_KEY_F10, InputSettings::NativeKeyboard::F10},
        {SCITER_KEY_F11, InputSettings::NativeKeyboard::F11},
        {SCITER_KEY_F12, InputSettings::NativeKeyboard::F12},
        {SCITER_KEY_RIGHT, InputSettings::NativeKeyboard::Right},
        {SCITER_KEY_LEFT, InputSettings::NativeKeyboard::Left},
        {SCITER_KEY_DOWN, InputSettings::NativeKeyboard::Down},
        {SCITER_KEY_UP, InputSettings::NativeKeyboard::Up},
        {SCITER_KEY_F13, InputSettings::NativeKeyboard::F13},
        {SCITER_KEY_F14, InputSettings::NativeKeyboard::F14},
        {SCITER_KEY_F15, InputSettings::NativeKeyboard::F15},
        {SCITER_KEY_F16, InputSettings::NativeKeyboard::F16},
        {SCITER_KEY_F17, InputSettings::NativeKeyboard::F17},
        {SCITER_KEY_F18, InputSettings::NativeKeyboard::F18},
        {SCITER_KEY_F19, InputSettings::NativeKeyboard::F19},
        {SCITER_KEY_F20, InputSettings::NativeKeyboard::F20},
        {SCITER_KEY_F21, InputSettings::NativeKeyboard::F21},
        {SCITER_KEY_F22, InputSettings::NativeKeyboard::F22},
        {SCITER_KEY_F23, InputSettings::NativeKeyboard::F23},
        {SCITER_KEY_F24, InputSettings::NativeKeyboard::F24},
    };

    if (keyMappings.find(key) != keyMappings.end())
    {
        return keyMappings.at(key);
    }
    return 0;
}

int32_t SciterKeyToVKCode(SciterKeys key)
{
    static const std::unordered_map<uint32_t, int32_t> keyMappings = {
        {'A', 'A'},
        {'B', 'B'},
        {'C', 'C'},
        {'D', 'D'},
        {'E', 'E'},
        {'F', 'F'},
        {'G', 'G'},
        {'H', 'H'},
        {'I', 'I'},
        {'J', 'J'},
        {'K', 'K'},
        {'L', 'L'},
        {'M', 'M'},
        {'N', 'N'},
        {'O', 'O'},
        {'P', 'P'},
        {'Q', 'Q'},
        {'R', 'R'},
        {'S', 'S'},
        {'T', 'T'},
        {'U', 'U'},
        {'V', 'V'},
        {'W', 'W'},
        {'X', 'X'},
        {'Y', 'Y'},
        {'Z', 'Z'},
        {'1', '1'},
        {'2', '2'},
        {'3', '3'},
        {'4', '4'},
        {'5', '5'},
        {'6', '6'},
        {'7', '7'},
        {'8', '8'},
        {'9', '9'},
        {'0', '0'},
        {SCITER_KEY_SPACE, VK_SPACE},
        {SCITER_KEY_ENTER, VK_RETURN},
        {SCITER_KEY_ESCAPE, VK_ESCAPE},
        {SCITER_KEY_TAB, VK_TAB},
        {SCITER_KEY_BACKSPACE, VK_BACK},
        {SCITER_KEY_F1, VK_F1},
        {SCITER_KEY_F2, VK_F2},
        {SCITER_KEY_F3, VK_F3},
        {SCITER_KEY_F4, VK_F4},
        {SCITER_KEY_F5, VK_F5},
        {SCITER_KEY_F6, VK_F6},
        {SCITER_KEY_F7, VK_F7},
        {SCITER_KEY_F8, VK_F8},
        {SCITER_KEY_F9, VK_F9},
        {SCITER_KEY_F10, VK_F10},
        {SCITER_KEY_F11, VK_F11},
        {SCITER_KEY_F12, VK_F12},
        {SCITER_KEY_RIGHT, VK_RIGHT},
        {SCITER_KEY_LEFT, VK_LEFT},
        {SCITER_KEY_DOWN, VK_DOWN},
        {SCITER_KEY_UP, VK_UP},
        {SCITER_KEY_F13, VK_F13},
        {SCITER_KEY_F14, VK_F14},
        {SCITER_KEY_F15, VK_F15},
        {SCITER_KEY_F16, VK_F16},
        {SCITER_KEY_F17, VK_F17},
        {SCITER_KEY_F18, VK_F18},
        {SCITER_KEY_F19, VK_F19},
        {SCITER_KEY_F20, VK_F20},
        {SCITER_KEY_F21, VK_F21},
        {SCITER_KEY_F22, VK_F22},
        {SCITER_KEY_F23, VK_F23},
        {SCITER_KEY_F24, VK_F24},
    };

    if (keyMappings.find(key) != keyMappings.end())
    {
        return keyMappings.at(key);
    }
    return 0;
}

std::string KeyCodeToString(int keyCode)
{
    static const std::unordered_map<int, std::string> keyMap = {
        {VK_BACK, "Backspace"},
        {VK_TAB, "Tab"},
        {VK_RETURN, "Enter"},
        {VK_SHIFT, "Shift"},
        {VK_CONTROL, "Ctrl"},
        {VK_MENU, "Alt"},
        {VK_PAUSE, "Pause"},
        {VK_CAPITAL, "Caps Lock"},
        {VK_ESCAPE, "Esc"},
        {VK_SPACE, "Space"},
        {VK_PRIOR, "Page Up"},
        {VK_NEXT, "Page Down"},
        {VK_END, "End"},
        {VK_HOME, "Home"},
        {VK_LEFT, "Left"},
        {VK_UP, "Up"},
        {VK_RIGHT, "Right"},
        {VK_DOWN, "Down"},
        {VK_INSERT, "Insert"},
        {VK_DELETE, "Delete"},
        // 0-9 keys
        {'0', "0"}, {'1', "1"}, {'2', "2"}, {'3', "3"}, {'4', "4"},
        {'5', "5"}, {'6', "6"}, {'7', "7"}, {'8', "8"}, {'9', "9"},
        // A-Z keys
        {'A', "A"}, {'B', "B"}, {'C', "C"}, {'D', "D"}, {'E', "E"},
        {'F', "F"}, {'G', "G"}, {'H', "H"}, {'I', "I"}, {'J', "J"},
        {'K', "K"}, {'L', "L"}, {'M', "M"}, {'N', "N"}, {'O', "O"},
        {'P', "P"}, {'Q', "Q"}, {'R', "R"}, {'S', "S"}, {'T', "T"},
        {'U', "U"}, {'V', "V"}, {'W', "W"}, {'X', "X"}, {'Y', "Y"},
        {'Z', "Z"},
        // Function keys
        {VK_F1, "F1"}, {VK_F2, "F2"}, {VK_F3, "F3"}, {VK_F4, "F4"},
        {VK_F5, "F5"}, {VK_F6, "F6"}, {VK_F7, "F7"}, {VK_F8, "F8"},
        {VK_F9, "F9"}, {VK_F10, "F10"}, {VK_F11, "F11"}, {VK_F12, "F12"},
        // Numpad
        {VK_NUMPAD0, "Num 0"}, {VK_NUMPAD1, "Num 1"}, {VK_NUMPAD2, "Num 2"},
        {VK_NUMPAD3, "Num 3"}, {VK_NUMPAD4, "Num 4"}, {VK_NUMPAD5, "Num 5"},
        {VK_NUMPAD6, "Num 6"}, {VK_NUMPAD7, "Num 7"}, {VK_NUMPAD8, "Num 8"},
        {VK_NUMPAD9, "Num 9"},
        {VK_MULTIPLY, "Num *"}, {VK_ADD, "Num +"}, {VK_SUBTRACT, "Num -"},
        {VK_DECIMAL, "Num ."}, {VK_DIVIDE, "Num /"},
        // Other keys
        {VK_OEM_1, ";"}, {VK_OEM_PLUS, "="}, {VK_OEM_COMMA, ","},
        {VK_OEM_MINUS, "-"}, {VK_OEM_PERIOD, "."}, {VK_OEM_2, "/"},
        {VK_OEM_3, "`"}, {VK_OEM_4, "["}, {VK_OEM_5, "\\"},
        {VK_OEM_6, "]"}, {VK_OEM_7, "'"},
    };

    std::unordered_map<int, std::string>::const_iterator it = keyMap.find(keyCode);
    if (it != keyMap.end())
    {
        return it->second;
    }
    return std::to_string(keyCode);
}
