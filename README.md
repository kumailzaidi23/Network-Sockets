# Network Sockets in Windows

Welcome to the Network Sockets in Windows repository. This project demonstrates the basics of socket programming on the Windows platform using C++.

## Overview

Socket programming is essential for network communication. This repository includes examples and explanations of how to create, bind, listen, and accept connections using Windows sockets.

## Prerequisites

- Visual Studio installed
- Basic knowledge of C++
- Windows SDK (included with Visual Studio)

## Getting Started

### Cloning the Repository

```bash
git clone https://github.com/kumailzaidi23/Network-Sockets.git
cd Network-Sockets
```

## Setting Up Your Environment
1. Open the cloned repository in Visual Studio.
2. Ensure you have the necessary libraries and headers for Windows socket programming.

## Installing Required Libraries
To set up the required libraries, you need to include the necessary headers and link the appropriate library in your project. The key headers and library needed are:
```cpp
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#pragma comment(lib, "ws2_32.lib")
```
## Configuring Visual Studio

**Include Directories**: Make sure the directories containing `WinSock2.h`, `WS2tcpip.h`, and `Windows.h` are included in your project. These headers are typically found in the Windows SDK.

**Linker Settings**: Link against the `ws2_32.lib` library. You can do this in Visual Studio by:
- Right-clicking on your project in Solution Explorer and selecting `Properties`.
- Navigating to `Configuration Properties` > `Linker` > `Input`.
- Adding `ws2_32.lib` to the `Additional Dependencies` field.

## Running the Example

- Build the project by selecting `Build` > `Build Solution` in Visual Studio.
- Run the executable. It will create a socket, bind it to a port, and start listening for incoming connections.

## Code Breakdown

- `server.cpp`: Implements the server-side code to accept and handle client connections.
- `client.cpp`: Implements the client-side code to connect to the server and send/receive data.

## Key Functions

- `socket()`: Creates a socket.
- `bind()`: Binds the socket to a local IP address and port.
- `listen()`: Listens for incoming connections.
- `accept()`: Accepts an incoming connection from a client.
- `send()`: Sends data through the socket.
- `recv()`: Receives data from the socket.

## References

- [Microsoft Windows Sockets 2 Documentation](https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-start-page-2)
- [Beej's Guide to Network Programming](http://beej.us/guide/bgnet/)

## Contributing

Feel free to fork this repository and submit pull requests for any improvements or additional features.

## License

This project is licensed under the MIT License. See the `LICENSE` file for more details.
