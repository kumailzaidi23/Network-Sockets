#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <tchar.h>
#include <iostream>                
using namespace std;
#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData; 
    /*WSADATA is a structure used in the Windows Sockets (Winsock) API to 
     store details about the initialization of the Winsock library.*/

    int port = 443; 
    int wsaerr;

    WORD Version = MAKEWORD(2, 2);
    /* WORD is a data type of 16 bits. */

    wsaerr = WSAStartup(Version, &wsaData); // INITIALIZING WSA (MOST IMPORTANT)

    /* passing the WORD which is data type and a pointer of 
     structure WSADATA(stores version of implementation that
     that win sock stores.*/

    if (wsaerr != 0) {
        cout << "Failed to initialize Winsock. Error code: " << wsaerr << endl;
        return 0;
    }
    else {
        cout << "The Winsock DLL Found!" << endl << "Status Code: " << wsaData.szSystemStatus << endl;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //This line creates a stream socket 
                                                                    //(SOCK_STREAM) for TCP communication
                                                                    //(IPPROTO_TCP) in the IPv4 address 
                                                                    //family (AF_INET).

    /*SOCKET is not a structure but a handle or identifier.
    It's typically implemented as a data type that represents 
    a reference or pointer to an internal data structure maintained
    by the networking library. The actual structure and details of 
    this internal data structure are typically hidden from the 
    application developer.*/

    /* In the Winsock API, a SOCKET is defined as an 
    unsigned integral type, and you are not meant to access
    or manipulate the internal details directly.Instead, you 
    perform operations on sockets using functions provided 
    by the Winsock library.*/

    if (serverSocket == INVALID_SOCKET) {
        cout << "ERROR AT SOCKET(): " << WSAGetLastError() << endl;
        WSACleanup();
        return 0;
    }
    else {
        cout << "Socket() is OK!" << endl;
    }

    sockaddr_in service;

    /*sockaddr_in is a structure for representing an Internet socket address
    structure, specifically for IPv4 addresses.*/

    service.sin_family = AF_INET; // object of sockaddr_in which is address family Internet Network.

    InetPton(AF_INET, _T("127.0.0.1"), &service.sin_addr);

    /*InetPton is a function used to convert a string
    representation of an IP address to its numeric form(binary form).

    _T("127.0.0.1") is a macro
     */
    
    /*The sin_addr member in the sockaddr_in structure is 
    of type struct in_addr, which represents an IPv4 address.
    it breaks the 4 bytes of IP in 4 different bytes*/

    /*InetPton is used to convert the string "127.0.0.1" to 
    its binary form and store it in the sin_addr member of 
    a sockaddr_in structure.*/

    service.sin_port = htons(port);

    /*htons is a function for "host to network short." 
    This function is used to convert a 16 - bit quantity
    (such as a port number) from host byte order to network byte order.*/

    if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {

        /*bind associate a local address(IP address and port number) with a socket

        serverSocket: The socket to be bound. This is the socket created using the socket function.

        (SOCKADDR*)&service: The second parameter is a pointer to a SOCKADDR structure that represents
        the local address to bind to. it's a pointer to the service variable of type sockaddr_in.
        The (SOCKADDR*) is a typecast to the generic socket address type.

        sizeof(service): The third parameter specifies the size of the SOCKADDR structure. 
        it's the size of the sockaddr_in structure, determined by the sizeof operator.
        */

        cout << "bind() failed. Error code: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 0;
    }
    else {
        cout << "Bind() is OK!" << endl;
    }

    if (listen(serverSocket, 1) == SOCKET_ERROR) {

        /*After calling listen, the server socket is in a listening state, 
        and it can accept incoming connections using the accept function which is below.
        The listen function prepares the socket to accept incoming connections,
        and the parameter in this case is 1, which means it will listen to only
        one connection.
        */

        cout << "listen(): error listening on socket " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 0;
    }
    else {
        cout << "Listen() is OK!" << endl;
    }

    /*serverSocket: The listening socket (previously created, bound, and put in a 
    listening state using socket, bind, and listen).

    NULL, NULL: The second and third parameters are pointers to sockaddr structures that 
    will receive the address information of the connecting client. In this case, they are set to 
    NULL because the server might not be interested in the client's address.
    
    acceptsocket: The variable that will hold the new socket created for the accepted connection. 
    This socket can be used for communication with the connected client.*/

    while (true) {
        SOCKET acceptsocket;
        acceptsocket = accept(serverSocket, NULL, NULL);

        if (acceptsocket == INVALID_SOCKET) {
            cout << "accept failed " << WSAGetLastError() << endl;
            closesocket(serverSocket);
            WSACleanup();
            return 0;
        }

        cout << "!!!!CONNECTION ESTABLISHED!!!!" << endl;

        char message[200] = "";
        int bytes = recv(acceptsocket, message, 200, 0);
        if (bytes < 0) {
            printf("client: Send error %ld.\n", WSAGetLastError());
            return -1;
        }
        else {
            cout << endl;
            cout << "The Message Send By Client is: " << "' " << message << " '" << endl;
        }

        closesocket(acceptsocket); // Close the client socket after handling
    }

    // Cleanup code (though it won't be reached due to the infinite loop)
    WSACleanup();
    return 0;
}