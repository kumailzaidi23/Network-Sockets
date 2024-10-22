#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <tchar.h>
#include <iostream>
#include <string>

using namespace std;
#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    int port = 443;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);

    wsaerr = WSAStartup(wVersionRequested, &wsaData);
    if (wsaerr != 0) {
        cout << "Failed to initialize Winsock. Error code: " << wsaerr << endl;
        return 0;
    }
    else {
        cout << "The Winsock DLL found!" << endl << "The status code is " << wsaData.szSystemStatus << endl;
    }

    while (true) {
        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (clientSocket == INVALID_SOCKET) {
            cout << "ERROR AT SOCKET(): " << WSAGetLastError() << endl;
            WSACleanup();
            return 0;
        }
        else {
            cout << "socket() is ok" << endl;
        }

        string domain;
        cout << "Enter domain name (or 'exit' to quit): ";
        getline(cin, domain);

        if (domain == "exit") {
            break;
        }

        struct addrinfo hints, * result;
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        int res = getaddrinfo(domain.c_str(), to_string(port).c_str(), &hints, &result);
        if (res != 0) {
            cout << "getaddrinfo failed: " << gai_strerror(res) << endl;
            closesocket(clientSocket);
            continue;
        }

        for (struct addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
            char ipstr[INET_ADDRSTRLEN];
            sockaddr_in* ipv4 = (sockaddr_in*)ptr->ai_addr;
            inet_ntop(ptr->ai_family, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
            cout << "Resolved IP address: " << ipstr << endl;

            sockaddr_in clientservice;
            clientservice.sin_family = AF_INET;
            clientservice.sin_addr = ipv4->sin_addr;
            clientservice.sin_port = htons(port);

            if (connect(clientSocket, (SOCKADDR*)&clientservice, sizeof(clientservice)) == SOCKET_ERROR) {
                cout << "Client failed to connect. Error code: " << WSAGetLastError() << endl;
                closesocket(clientSocket);
                continue;
            }
            else {
                cout << "Client: Connect() is okay. Can send data now." << endl;
                break;
            }
        }

        freeaddrinfo(result);

        char message[200];
        cout << "Enter Your Message: ";
        cin.getline(message, 200);

        int bytes = send(clientSocket, message, strlen(message), 0);
        if (bytes == SOCKET_ERROR) {
            printf("Server: Send error %ld.\n", WSAGetLastError());
        }
        else {
            printf("Server: Sent %ld bytes.\n", bytes);
        }

        closesocket(clientSocket);
    }

    WSACleanup();
    return 0;
}
