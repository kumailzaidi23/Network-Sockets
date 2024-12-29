#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>

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

void receiveMessages(SOCKET clientSocket) {
    char buffer[512];
    while (true) {
        int bytes = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
            cout << "Disconnected from server. Error: " << WSAGetLastError() << endl;
            closesocket(clientSocket);
            WSACleanup();
            exit(1);
        }
        string encryptedMessage(buffer, bytes);
        string message = xorEncryptDecrypt(encryptedMessage);
        cout << "Server: " << message << endl;
    }
}

int main(int argc, char* argv[]) {
    string serverIpAddress = "127.0.0.1"; // Default server IP address
    int port = 443; // Default port
    string clientIpAddress = "127.0.0.5"; // Default client IP address (bind to any available interface)

    if (argc > 1) {
        serverIpAddress = argv[1];
    }
    if (argc > 2) {
        port = stoi(argv[2]);
    }
    if (argc > 3) {
        clientIpAddress = argv[3];
    }
    if (argc > 4) {
        key = argv[4];
    }

    WSADATA wsaData;
    int wsaerr;
    WORD Version = MAKEWORD(2, 2);
    wsaerr = WSAStartup(Version, &wsaData);

    if (wsaerr != 0) {
        cout << "Failed to initialize Winsock. Error code: " << wsaerr << endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        cout << "ERROR AT SOCKET(): " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in clientAddr;
    clientAddr.sin_family = AF_INET;
    InetPtonA(AF_INET, clientIpAddress.c_str(), &clientAddr.sin_addr);
    clientAddr.sin_port = 0; // Let the system choose any available port

    if (bind(clientSocket, (sockaddr*)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR) {
        cout << "bind() failed. Error code: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    InetPtonA(AF_INET, serverIpAddress.c_str(), &serverAddr.sin_addr);
    serverAddr.sin_port = htons(port);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "connect() failed. Error code: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    cout << "Connected to server at " << serverIpAddress << ":" << port << endl;

    thread receiveThread(receiveMessages, clientSocket);
    receiveThread.detach();

    string username;
    cout << "Enter your username: ";
    getline(cin, username);
    string encryptedUsername = xorEncryptDecrypt(username);
    send(clientSocket, encryptedUsername.c_str(), encryptedUsername.size(), 0);

    while (true) {
        string message;
        getline(cin, message);
        string encryptedMessage = xorEncryptDecrypt(message);
        send(clientSocket, encryptedMessage.c_str(), encryptedMessage.size(), 0);
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
