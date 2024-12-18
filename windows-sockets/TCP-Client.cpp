#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <thread>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

string key = "GroupOfFour";

string xorEncryptDecrypt(const string& input) {
    string output = input;
    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = input[i] ^ key[i % key.size()];
    }
    return output;
}

void receiveMessages(SOCKET serverSocket) {
    char buffer[512];
    while (true) {
        int bytes = recv(serverSocket, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
            cout << "Disconnected from server. Error: " << WSAGetLastError() << endl;
            closesocket(serverSocket);
            WSACleanup();
            exit(1);
        }
        else {
            string encryptedMessage(buffer, bytes);
            string message = xorEncryptDecrypt(encryptedMessage);
            cout << "Received: " << message << endl;
        }
    }
}

int main() {
    WSADATA wsaData;
    int wsaerr;
    WORD Version = MAKEWORD(2, 2);
    wsaerr = WSAStartup(Version, &wsaData);

    if (wsaerr != 0) {
        cout << "Failed to initialize Winsock. Error code: " << wsaerr << endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cout << "ERROR AT SOCKET(): " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(443);
    InetPton(AF_INET, _T("127.0.0.1"), &serverAddr.sin_addr);

    if (connect(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Failed to connect to server. Error: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Connected to server!" << endl;

    thread receiveThread(receiveMessages, serverSocket);
    receiveThread.detach();

    while (true) {
        string message;
        getline(cin, message);

        if (message == "exit") {
            break;
        }

        if (message == "LIST") {
            string encryptedMessage = xorEncryptDecrypt(message);
            send(serverSocket, encryptedMessage.c_str(), encryptedMessage.size(), 0);
        }
        else {
            cout << "Enter target client and message (format: ClientX:message): ";
            getline(cin, message);
            string encryptedMessage = xorEncryptDecrypt(message);
            send(serverSocket, encryptedMessage.c_str(), encryptedMessage.size(), 0);
        }
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}