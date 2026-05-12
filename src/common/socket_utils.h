#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

int createServerSocket(int port) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0) {
        throw std::runtime_error("Cannot create server socket");
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        close(serverSocket);
        throw std::runtime_error("Cannot bind server socket");
    }

    if (listen(serverSocket, 10) < 0) {
        close(serverSocket);
        throw std::runtime_error("Cannot listen server socket");
    }

    return serverSocket;
}

int acceptClient(int serverSocket) {
    sockaddr_in clientAddress{};
    socklen_t clientAddressSize = sizeof(clientAddress);

    int clientSocket = accept(
        serverSocket,
        reinterpret_cast<sockaddr*>(&clientAddress),
        &clientAddressSize
    );

    if (clientSocket < 0) {
        throw std::runtime_error("Cannot accept client");
    }

    return clientSocket;
}

int connectToServer(const std::string& host, int port) {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket < 0) {
        throw std::runtime_error("Cannot create client socket");
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &serverAddress.sin_addr) <= 0) {
        close(clientSocket);
        throw std::runtime_error("Invalid server address");
    }

    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        close(clientSocket);
        throw std::runtime_error("Cannot connect to server");
    }

    return clientSocket;
}

void sendLine(int socketFd, const std::string& line) {
    std::string message = line + "\n";
    const char* data = message.c_str();
    std::size_t totalSent = 0;

    while (totalSent < message.size()) {
        ssize_t sent = send(socketFd, data + totalSent, message.size() - totalSent, 0);

        if (sent <= 0) {
            throw std::runtime_error("Cannot send data");
        }

        totalSent += static_cast<std::size_t>(sent);
    }
}

std::string readLine(int socketFd) {
    std::string line;
    char c = '\0';

    while (true) {
        ssize_t received = recv(socketFd, &c, 1, 0);

        if (received <= 0) {
            throw std::runtime_error("Connection closed");
        }

        if (c == '\n') {
            break;
        }

        line += c;
    }

    return line;
}

void closeSocket(int socketFd) {
    close(socketFd);
}