# Encrypted Multi-Client Chat Server

This project implements a multi-client chat server using C++ and Winsock2 for Windows systems. The server allows authenticated communication between clients and provides several administrative commands for enhanced functionality and control.

## Features

- **Multi-Client Support:** Handle multiple clients simultaneously using threads.
- **Encrypted Communication:** Messages are encrypted and decrypted using an XOR-based encryption algorithm.
- **Administrative Commands:** Support for user management, including muting, unmuting, kicking users, and broadcasting messages.
- **Command List:**
  - `LIST` - Lists all connected clients.
  - `BROADCAST:<message>` - Sends a broadcast message to all clients.
  - `WHISPER:<username>:<message>` - Sends a private message to a specific user.
  - `CHANGEUSERNAME:<new_username>` - Changes the user's display name.
  - `SHUTDOWN` - Shuts down the server (admin only).
  - `KICK:<username>` - Removes a user from the server (admin only).
  - `MUTE:<username>` - Mutes a user (admin only).
  - `UNMUTE:<username>` - Unmutes a user (admin only).
  
## Requirements

- **Windows OS**
- **Compiler:** Visual Studio or any compatible C++ compiler that supports Winsock2.
- **Libraries:**
  - Winsock2 (`ws2_32.lib`)

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/kumailzaidi23/Network-Sockets
   cd Network-Sockets
   ```

2. Open the project in your preferred C++ IDE (e.g., Visual Studio).

3. Build and compile the project.

## Usage

### Starting the Server

Run the server executable from the command line:

```bash
TCPServer.exe [ipAddress] [port] [encryptionKey]
```

- `ipAddress`: The IP address to bind the server (default is `127.0.0.1`).
- `port`: The port number to listen for connections (default is `443`).
- `encryptionKey`: The encryption key for message encryption (default is `GroupOfFour`).

### Example

```bash
TCPServer.exe 127.0.0.1 443 MySecretKey
```

## Security

- **Encryption:** The XOR encryption algorithm is used for message security.
- **Admin Commands:** The `admin` user has access to privileged commands such as server shutdown and user management.

## Known Issues

- The XOR encryption used is simple and may not provide strong security.
- The server does not currently implement advanced error handling for all scenarios.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request for any improvements or feature suggestions.

## Contact

For questions or support, please contact [kkomailzaidi23@gmail.com].

