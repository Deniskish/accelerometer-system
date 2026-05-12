#include "../common/socket_utils.h"

#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[]) {
    try {
        if (argc < 3) {
            std::cerr << "Usage: ./node_b <server_host> <server_port>" << std::endl;
            return 1;
        }

        std::string serverHost = argv[1];
        int serverPort = std::stoi(argv[2]);

        std::cout << "Node B started" << std::endl;
        std::cout << "Connecting to " << serverHost << ":" << serverPort << std::endl;

        int socketFd = connectToServer(serverHost, serverPort);

        std::cout << "Connected to server" << std::endl;

        while (true) {
            std::string message = readLine(socketFd);

            std::cout << "Received from server: " << message << std::endl;
        }

        closeSocket(socketFd);
    } catch (const std::exception& exception) {
        std::cerr << "Node B error: " << exception.what() << std::endl;
        return 1;
    }

    return 0;
}