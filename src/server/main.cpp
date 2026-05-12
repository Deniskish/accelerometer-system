#include "../common/socket_utils.h"

#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[]) {
    try {
        if (argc < 4) {
            std::cerr << "Usage: ./server <host> <port_a> <port_b>" << std::endl;
            return 1;
        }

        int portA = std::stoi(argv[2]);
        int portB = std::stoi(argv[3]);

        std::cout << "Server started" << std::endl;

        int serverSocketA = createServerSocket(portA);
        int serverSocketB = createServerSocket(portB);

        std::cout << "Waiting for Node A on port " << portA << std::endl;
        int nodeASocket = acceptClient(serverSocketA);
        std::cout << "Node A connected" << std::endl;

        std::cout << "Waiting for Node B on port " << portB << std::endl;
        int nodeBSocket = acceptClient(serverSocketB);
        std::cout << "Node B connected" << std::endl;

        while (true) {
            std::string messageFromA = readLine(nodeASocket);

            std::cout << "Received from Node A: " << messageFromA << std::endl;

            sendLine(nodeBSocket, messageFromA);

            std::cout << "Forwarded to Node B" << std::endl;
        }

        closeSocket(nodeASocket);
        closeSocket(nodeBSocket);
        closeSocket(serverSocketA);
        closeSocket(serverSocketB);
    } catch (const std::exception& exception) {
        std::cerr << "Server error: " << exception.what() << std::endl;
        return 1;
    }

    return 0;
}