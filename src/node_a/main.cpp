#include "../common/socket_utils.h"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

int main(int argc, char* argv[]) {
    try {
        if (argc < 3) {
            std::cerr << "Usage: ./node_a <server_host> <server_port> <frequency_hz> <log_file>" << std::endl;
            return 1;
        }

        std::string serverHost = argv[1];
        int serverPort = std::stoi(argv[2]);

        std::cout << "Node A started" << std::endl;
        std::cout << "Connecting to " << serverHost << ":" << serverPort << std::endl;

        int socketFd = connectToServer(serverHost, serverPort);

        std::cout << "Connected to server" << std::endl;

        int counter = 0;

        while (true) {
            std::string message = "{\"test\":\"hello from node A\", \"counter\":" + std::to_string(counter) + "}";

            sendLine(socketFd, message);

            std::cout << "Sent: " << message << std::endl;

            ++counter;

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        closeSocket(socketFd);
    } catch (const std::exception& exception) {
        std::cerr << "Node A error: " << exception.what() << std::endl;
        return 1;
    }

    return 0;
}