#include "../common/accel_packet.h"
#include "../common/socket_utils.h"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

int roundToThousand(double value) {
    return static_cast<int>(std::round(value * 1000.0));
}

bool isDuplicatePacket(const AccelPacket& currentPacket,
                       const AccelPacket& previousPacket,
                       bool hasPreviousPacket) {
    if (!hasPreviousPacket) {
        return false;
    }

    return roundToThousand(currentPacket.x) == roundToThousand(previousPacket.x) &&
           roundToThousand(currentPacket.y) == roundToThousand(previousPacket.y) &&
           roundToThousand(currentPacket.z) == roundToThousand(previousPacket.z);
}

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

        AccelPacket previousPacket{};
        bool hasPreviousPacket = false;

        while (true) {
            std::string messageFromA = readLine(nodeASocket);

            std::cout << "Received from Node A: " << messageFromA << std::endl;

            AccelPacket currentPacket = parseAccelPacket(messageFromA);

            if (isDuplicatePacket(currentPacket, previousPacket, hasPreviousPacket)) {
                std::cout << "Duplicate packet skipped" << std::endl;
                continue;
            }

            previousPacket = currentPacket;
            hasPreviousPacket = true;

            sendLine(nodeBSocket, messageFromA);

            std::cout << "Forwarded packet to Node B" << std::endl;

            std::string moduleFromB = readLine(nodeBSocket);

            std::cout << "Received from Node B: " << moduleFromB << std::endl;

            sendLine(nodeASocket, moduleFromB);

            std::cout << "Forwarded module to Node A" << std::endl;
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