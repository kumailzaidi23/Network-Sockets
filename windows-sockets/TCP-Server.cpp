#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <tchar.h>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <map>
#include <string>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

map<SOCKET, string> clients;
mutex clientsMutex;

string key = "GroupOfFour";

string xorEncryptDecrypt(const string& input) {
    string output = input;
    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = input[i] ^ key[i % key.size()];
    }
    return output;
}

void sendToClient(SOCKET clientSocket, const string& message) {
    string encryptedMessage = xorEncryptDecrypt(message);
    send(clientSocket, encryptedMessage.c_str(), encryptedMessage.size(), 0);
    cout << "Sent to client " << clients[clientSocket] << ": " << message << endl;
}

void handleClient(SOCKET clientSocket) {
    char buffer[512] = "";
    while (true) {
        int bytes = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
            cout << "Client " << clients[clientSocket] << " disconnected. Error: " << WSAGetLastError() << endl;
            closesocket(clientSocket);
            break;
        }
        else {
            string encryptedMessage(buffer, bytes);
            string message = xorEncryptDecrypt(encryptedMessage);
            cout << "Received from client " << clients[clientSocket] << ": " << message << endl;

            if (message == "LIST") {
                lock_guard<mutex> lock(clientsMutex);
                string clientList = "Connected clients:\n";
                for (const auto& client : clients) {
                    clientList += client.second + "\n";
                }
                sendToClient(clientSocket, clientList);
            }
            else {
                size_t pos = message.find(':');
                if (pos != string::npos) {
                    string targetClient = message.substr(0, pos);
                    string clientMessage = message.substr(pos + 1);

                    lock_guard<mutex> lock(clientsMutex);
                    for (const auto& client : clients) {
                        if (client.second == targetClient) {
                            sendToClient(client.first, clientMessage);
                            break;
                        }
                    }
                }
            }
        }
    }

    // Remove client from the list
    lock_guard<mutex> lock(clientsMutex);
    clients.erase(clientSocket);
}

void startServer(int port) {
    WSADATA wsaData;
    int wsaerr;
    WORD Version = MAKEWORD(2, 2);
    wsaerr = WSAStartup(Version, &wsaData);

    if (wsaerr != 0) {
        cout << "Failed to initialize Winsock. Error code: " << wsaerr << endl;
        return;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cout << "ERROR AT SOCKET(): " << WSAGetLastError() << endl;
        WSACleanup();
        return;
    }

    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(port);

    if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        cout << "bind() failed. Error code: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cout << "listen() failed. Error code: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    cout << "Server started on port " << port << endl;

    while (true) {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            cout << "accept() failed. Error code: " << WSAGetLastError() << endl;
            closesocket(serverSocket);
            WSACleanup();
            return;
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        cout << "Client connected from IP: " << clientIP << endl;

        {
            lock_guard<mutex> lock(clientsMutex);
            clients[clientSocket] = "Client" + to_string(clientSocket);
        }
        thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }

    closesocket(serverSocket);
    WSACleanup();
}

int main(int argc, char* argv[]) {
    int port = 443; // Default port
    if (argc > 1) {
        port = stoi(argv[1]);
    }
    if (argc > 2) {
        key = argv[2];
    }

    cout << "Starting server on port " << port << " with encryption key: " << key << endl;
    startServer(port);
    return 0;
}