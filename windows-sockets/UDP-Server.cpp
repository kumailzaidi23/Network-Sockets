#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <tchar.h>
#include <iostream>

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

    SOCKET serversocket;
    serversocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serversocket == INVALID_SOCKET) {
        cout << "INVALID SOCKET! ERROR CODE: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }
    else {
        cout << "SOCKET IS OK! " << endl;
    }

    sockaddr_in service;
    service.sin_family = AF_INET;
    InetPton(AF_INET, _T("127.0.0.1"), &service.sin_addr);
    service.sin_port = htons(port);

    if (bind(serversocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        cout << "bind() failed. Error code: " << WSAGetLastError() << endl;
        closesocket(serversocket);
        WSACleanup();
        return 1;
    }
    else {
        cout << "BIND IS OK!" << endl;
    }

    while (true) {
        char message[100] = { 0 };  // Initialize the buffer
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);

        int recvResult = recvfrom(serversocket, message, sizeof(message), 0, (SOCKADDR*)&clientAddr, &clientAddrSize);

        if (recvResult == SOCKET_ERROR) {
            cout << "recvfrom() failed. Error code: " << WSAGetLastError() << endl;
            break;
        }
        else {
            // Print the received message
            cout << "Received Message: " << message << endl;
        }
    }
    closesocket(serversocket);
    WSACleanup();

    return 0;
}
