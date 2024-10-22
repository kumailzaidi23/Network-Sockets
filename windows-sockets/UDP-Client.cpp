#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <tchar.h>
#include <iostream>
#include <string.h>
#include <string>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsadata;
    int port = 55555;
    int wsaerr;
    WORD version = MAKEWORD(2, 2);

    wsaerr = WSAStartup(version, &wsadata);
    if (wsaerr != 0) {
        cout << "WINDOWS SOCK DLL NOT FOUND! ERROR CODE: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }
    else {
        cout << "WINDOWS SOCK DLL FOUND!!" << endl;
        cout << "STATUS CODE: " << wsadata.szSystemStatus << endl;
    }

    // Create socket
    SOCKET clientSocket;
    clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET) {
        cout << "INVALID SOCKET! ERROR CODE: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }
    else {
        cout << "SOCKET IS OK!" << endl;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;

    // Get IP address or domain name from user
    string input;
    cout << "Enter IP address or domain name: ";
    getline(cin, input);

    // Resolve the input
    struct addrinfo hints, * res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP

    int result = getaddrinfo(input.c_str(), nullptr, &hints, &res);
    if (result != 0) {
        cout << "getaddrinfo failed: " << gai_strerror(result) << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Copy resolved address to serverAddr
    memcpy(&serverAddr, res->ai_addr, res->ai_addrlen);
    serverAddr.sin_port = htons(port);

    // Print resolved IP address
    char ipStr[INET_ADDRSTRLEN]; // Buffer for the IP address string
    inet_ntop(AF_INET, &((sockaddr_in*)res->ai_addr)->sin_addr, ipStr, sizeof(ipStr));
    cout << "Resolved IP Address: " << ipStr << endl;

    char buffer[100];

    while (true) {
        string message;
        cout << "Enter message to send (or type 'exit' to quit): ";
        getline(cin, message);

        if (message == "exit") {
            break; // Exit the loop if the user types 'exit'
        }

        if (message.length() > 100) {
            cout << "Message is too long! Keep it under 100 characters." << endl;
            continue;
        }

        memcpy(buffer, message.c_str(), message.length());
        memset(buffer + message.length(), 0, 100 - message.length());

        int sentBytes = sendto(clientSocket, buffer, 100, 0, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

        if (sentBytes == SOCKET_ERROR) {
            cout << "sendto() failed. Error code: " << WSAGetLastError() << endl;
        }
        else {
            cout << "Data packet sent successfully!" << endl;
        }
    }

    freeaddrinfo(res); // Free the address info
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
