#ifndef NETWORK_H
#define NETWORK_H

#include <string>
#include <vector>

enum class NetworkMode {
    NONE,
    SERVER,
    CLIENT
};

struct PacketData {
    std::string type;
    float data[8];
};

class Network {
private:
    void* host;
    void* peer;
    NetworkMode mode;
    bool connected;
    std::vector<PacketData> receivedPackets;

public:
    Network();
    ~Network();

    bool initServer(int port = 12345);
    bool initClient(const char* hostAddress, int port = 12345);
    void disconnect();
    void sendPacket(const std::string& data);
    void sendPacket(const char* data, size_t length);
    void pollEvents();
    bool isConnected() const { return connected; }
    bool isServer() const { return mode == NetworkMode::SERVER; }
    NetworkMode getMode() const { return mode; }

    std::vector<PacketData> getPackets();
    void clearPackets();
};

#endif