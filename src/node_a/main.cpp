#include "../common/accel_packet.h"
#include "../common/socket_utils.h"

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

AccelPacket generateAccelPacket(int counter) {
    AccelPacket packet{};

    double t = counter / 50.0;

    packet.version = 1;
    packet.timestamp = currentTimeMillis();

    packet.x = std::sin(t);
    packet.y = 9.8 + 0.1 * std::sin(t * 0.5);
    packet.z = 0.05 * std::cos(t);

    return packet;
}

void writeModuleToLog(std::ofstream& logFile, const AccelModule& module) {
    logFile << module.timestamp << " " << module.module << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        if (argc < 5) {
            std::cerr << "Usage: ./node_a <server_host> <server_port> <frequency_hz> <log_file>" << std::endl;
            return 1;
        }

        std::string serverHost = argv[1];
        int serverPort = std::stoi(argv[2]);
        int frequencyHz = std::stoi(argv[3]);
        std::string logFilePath = argv[4];

        if (frequencyHz <= 0) {
            std::cerr << "Frequency must be positive" << std::endl;
            return 1;
        }

        int intervalMs = 1000 / frequencyHz;

        std::ofstream logFile(logFilePath, std::ios::app);

        if (!logFile.is_open()) {
            std::cerr << "Cannot open log file: " << logFilePath << std::endl;
            return 1;
        }

        std::cout << "Node A started" << std::endl;
        std::cout << "Connecting to " << serverHost << ":" << serverPort << std::endl;
        std::cout << "Frequency: " << frequencyHz << " Hz" << std::endl;
        std::cout << "Log file: " << logFilePath << std::endl;

        int socketFd = connectToServer(serverHost, serverPort);

        std::cout << "Connected to server" << std::endl;

        int counter = 0;

        while (true) {
            AccelPacket packet = generateAccelPacket(counter);
            std::string message = accelPacketToJson(packet);

            sendLine(socketFd, message);

            std::cout << "Sent packet: " << message << std::endl;

            std::string response = readLine(socketFd);

            std::cout << "Received module: " << response << std::endl;

            AccelModule module = parseAccelModule(response);

            writeModuleToLog(logFile, module);

            ++counter;

            std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
        }

        closeSocket(socketFd);
    } catch (const std::exception& exception) {
        std::cerr << "Node A error: " << exception.what() << std::endl;
        return 1;
    }

    return 0;
}