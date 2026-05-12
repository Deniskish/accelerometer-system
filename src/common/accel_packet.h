#pragma once

#include <chrono>
#include <cmath>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <string>

struct AccelPacket {
    int version;
    std::int64_t timestamp;
    double x;
    double y;
    double z;
};

struct AccelModule {
    int version;
    std::int64_t timestamp;
    double module;
};

std::int64_t currentTimeMillis() {
    using namespace std::chrono;

    return duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    ).count();
}

std::string accelPacketToJson(const AccelPacket& packet) {
    std::ostringstream stream;

    stream << "{";
    stream << "\"version\":" << packet.version << ",";
    stream << "\"timestamp\":" << packet.timestamp << ",";
    stream << "\"x\":" << packet.x << ",";
    stream << "\"y\":" << packet.y << ",";
    stream << "\"z\":" << packet.z;
    stream << "}";

    return stream.str();
}

std::string accelModuleToJson(const AccelModule& module) {
    std::ostringstream stream;

    stream << "{";
    stream << "\"version\":" << module.version << ",";
    stream << "\"timestamp\":" << module.timestamp << ",";
    stream << "\"module\":" << module.module;
    stream << "}";

    return stream.str();
}

std::string extractJsonValue(const std::string& json, const std::string& key) {
    std::string pattern = "\"" + key + "\":";

    std::size_t start = json.find(pattern);

    if (start == std::string::npos) {
        throw std::runtime_error("JSON key not found: " + key);
    }

    start += pattern.size();

    std::size_t end = json.find_first_of(",}", start);

    if (end == std::string::npos) {
        throw std::runtime_error("Invalid JSON value for key: " + key);
    }

    return json.substr(start, end - start);
}

AccelPacket parseAccelPacket(const std::string& json) {
    AccelPacket packet{};

    packet.version = std::stoi(extractJsonValue(json, "version"));
    packet.timestamp = std::stoll(extractJsonValue(json, "timestamp"));
    packet.x = std::stod(extractJsonValue(json, "x"));
    packet.y = std::stod(extractJsonValue(json, "y"));
    packet.z = std::stod(extractJsonValue(json, "z"));

    return packet;
}

AccelModule parseAccelModule(const std::string& json) {
    AccelModule module{};

    module.version = std::stoi(extractJsonValue(json, "version"));
    module.timestamp = std::stoll(extractJsonValue(json, "timestamp"));
    module.module = std::stod(extractJsonValue(json, "module"));

    return module;
}

AccelModule calculateModule(const AccelPacket& packet) {
    AccelModule result{};

    result.version = packet.version;
    result.timestamp = packet.timestamp;
    result.module = std::sqrt(
        packet.x * packet.x +
        packet.y * packet.y +
        packet.z * packet.z
    );

    return result;
}