#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <tchar.h>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <map>
#include <set>
#include <string>

using namespace std;
#pragma comment(lib, "ws2_32.lib")

map<SOCKET, pair<string, string>> clients; // Map to store client socket, IP, and username
set<string> mutedUsers; // Set to store muted users
mutex clientsMutex;

string key = "GroupOfFour";
bool serverRunning = true;

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
    cout << "Sent to client " << clients[clientSocket].second << ": " << message << endl;
}

void broadcastMessage(const string& message) {
    lock_guard<mutex> lock(clientsMutex);
    for (const auto& client : clients) {
        sendToClient(client.first, message);
    }
}

void handleClient(SOCKET clientSocket) {
    char buffer[512] = "";
    bool authenticated = false;
    string username;

    while (true) {
        int bytes = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
            cout << "Client " << clients[clientSocket].second << " disconnected. Error: " << WSAGetLastError() << endl;
            closesocket(clientSocket);
            break;
        }
        else {
            string encryptedMessage(buffer, bytes);
            string message = xorEncryptDecrypt(encryptedMessage);
            cout << "Received from client: " << message << endl;

            if (!authenticated) {
                username = message;
                authenticated = true;
                {
                    lock_guard<mutex> lock(clientsMutex);
                    clients[clientSocket].second = username;
                }
                sendToClient(clientSocket, "Welcome " + username + "!");
                broadcastMessage(username + " has joined the chat.");
            }
            else {
                if (message == "LIST") {
                    lock_guard<mutex> lock(clientsMutex);
                    string clientList = "Connected clients:\n";
                    for (const auto& client : clients) {
                        clientList += client.second.second + " (" + client.second.first + ")\n";
                    }
                    sendToClient(clientSocket, clientList);
                }
                else if (message.find("BROADCAST:") == 0) {
                    if (mutedUsers.find(username) == mutedUsers.end()) {
                        string broadcastMsg = message.substr(10);
                        broadcastMessage("Broadcast from " + username + ": " + broadcastMsg);
                    }
                    else {
                        sendToClient(clientSocket, "You are muted and cannot send broadcast messages.");
                    }
                }
                else if (message == "SHUTDOWN") {
                    if (username == "admin") { // Only allow admin to shut down the server
                        serverRunning = false;
                        broadcastMessage("Server is shutting down...");
                        break;
                    }
                }
                else if (message == "HELP") {
                    string helpMessage = "Available commands:\n";
                    helpMessage += "LIST - List all connected clients\n";
                    helpMessage += "BROADCAST:<message> - Send a broadcast message\n";
                    helpMessage += "WHISPER:<username>:<message> - Send a private message\n";
                    helpMessage += "CHANGEUSERNAME:<new_username> - Change your username\n";
                    helpMessage += "SHUTDOWN - Shut down the server (admin only)\n";
                    helpMessage += "KICK:<username> - Kick a user from the server (admin only)\n";
                    helpMessage += "MUTE:<username> - Mute a user (admin only)\n";
                    helpMessage += "UNMUTE:<username> - Unmute a user (admin only)\n";
                    sendToClient(clientSocket, helpMessage);
                }
                else if (message.find("WHISPER:") == 0) {
                    size_t pos1 = message.find(':', 8);
                    if (pos1 != string::npos) {
                        string targetClient = message.substr(8, pos1 - 8);
                        string clientMessage = message.substr(pos1 + 1);

                        lock_guard<mutex> lock(clientsMutex);
                        bool found = false;
                        for (const auto& client : clients) {
                            if (client.second.second == targetClient) {
                                sendToClient(client.first, "Private message from " + username + ": " + clientMessage);
                                found = true;
                                break;
                            }
                        }
                        if (!found) {
                            sendToClient(clientSocket, "User " + targetClient + " not found.");
                        }
                    }
                }
                else if (message.find("CHANGEUSERNAME:") == 0) {
                    string newUsername = message.substr(15);
                    {
                        lock_guard<mutex> lock(clientsMutex);
                        clients[clientSocket].second = newUsername;
                    }
                    sendToClient(clientSocket, "Username changed to " + newUsername);
                    broadcastMessage(username + " has changed their username to " + newUsername);
                    username = newUsername;
                }
                else if (message.find("KICK:") == 0) {
                    if (username == "admin") {
                        string targetClient = message.substr(5);
                        lock_guard<mutex> lock(clientsMutex);
                        bool found = false;
                        for (const auto& client : clients) {
                            if (client.second.second == targetClient) {
                                sendToClient(client.first, "You have been kicked from the server.");
                                closesocket(client.first);
                                clients.erase(client.first);
                                broadcastMessage(targetClient + " has been kicked from the server.");
                                found = true;
                                break;
                            }
                        }
                        if (!found) {
                            sendToClient(clientSocket, "User " + targetClient + " not found.");
                        }
                    }
                }
                else if (message.find("MUTE:") == 0) {
                    if (username == "admin") {
                        string targetClient = message.substr(5);
                        lock_guard<mutex> lock(clientsMutex);
                        mutedUsers.insert(targetClient);
                        sendToClient(clientSocket, "User " + targetClient + " has been muted.");
                    }
                }
                else if (message.find("UNMUTE:") == 0) {
                    if (username == "admin") {
                        string targetClient = message.substr(7);
                        lock_guard<mutex> lock(clientsMutex);
                        mutedUsers.erase(targetClient);
                        sendToClient(clientSocket, "User " + targetClient + " has been unmuted.");
                    }
                }
                else {
                    sendToClient(clientSocket, "Unknown command. Type HELP for a list of available commands.");
                }
            }
        }
    }

    // Remove client from the list and notify others
    lock_guard<mutex> lock(clientsMutex);
    string disconnectedUser = clients[clientSocket].second;
    clients.erase(clientSocket);
    broadcastMessage(disconnectedUser + " has left the chat.");
}

void startServer(const string& ipAddress, int port) {
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
    wstring wIpAddress(ipAddress.begin(), ipAddress.end());
    InetPtonW(AF_INET, wIpAddress.c_str(), &service.sin_addr);
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

    cout << "Server started on IP " << ipAddress << " and port " << port << endl;

    while (serverRunning) {
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
            clients[clientSocket].first = clientIP;
        }
        thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }

    closesocket(serverSocket);
    WSACleanup();
}

int main(int argc, char* argv[]) {
    string ipAddress = "127.0.0.1"; // Default IP address (bind to all interfaces)
    int port = 443; // Default port

    if (argc > 1) {
        ipAddress = argv[1];
    }
    if (argc > 2) {
        port = stoi(argv[2]);
    }
    if (argc > 3) {
        key = argv[3];
    }

    cout << "Starting server on IP " << ipAddress << " and port " << port << " with encryption key: " << key << endl;
    startServer(ipAddress, port);
    return 0;
}
