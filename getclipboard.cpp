#include <iostream>
#include <fstream>
#include <windows.h>
#include <vector>
#include <string>
#include <cstring>
#include <thread>
#include <chrono>
#include <numeric> // For std::accumulate
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

void monitorClipboard(const std::string& textFilename, const std::string& imageFilename) {
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
                    GlobalUnlock(hData);
                }
            }
        }

        CloseClipboard();

        if (currentClipboardData != lastClipboardData) {
            lastClipboardData = currentClipboardData;
            saveClipboardDataToFile(textFilename, imageFilename); // Save new data
        }

        std::this_thread::sleep_for(std::chrono::seconds(1)); // Check every second
    }
}

int main() {
    std::string textFilename = "clipboard_text.txt";
    std::string imageFilename = "clipboard_image.txt";

    monitorClipboard(textFilename, imageFilename);

    return 0;
}
