#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <enet/enet.h>
#include "Network.h"
#include <iostream>
#include <cstring>

Network::Network() : host(nullptr), peer(nullptr), mode(NetworkMode::NONE), connected(false) {
    if (enet_initialize() != 0) {
        std::cerr << "Failed to initialize ENet" << std::endl;
    }
}

Network::~Network() {
    disconnect();
    enet_deinitialize();
}

bool Network::initServer(int port) {
    if (host != nullptr) {
        enet_host_destroy(reinterpret_cast<ENetHost*>(host));
        host = nullptr;
    }

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;

    host = reinterpret_cast<void*>(enet_host_create(&address, 2, 2, 0, 0));
    if (host == nullptr) {
        std::cerr << "Failed to create server host" << std::endl;
        return false;
    }

    mode = NetworkMode::SERVER;
    connected = false;
    std::cout << "Server started on port " << port << std::endl;
    return true;
}

bool Network::initClient(const char* hostAddress, int port) {
    if (host != nullptr) {
        enet_host_destroy(reinterpret_cast<ENetHost*>(host));
        host = nullptr;
    }

    host = reinterpret_cast<void*>(enet_host_create(nullptr, 2, 2, 0, 0));
    if (host == nullptr) {
        std::cerr << "Failed to create client host" << std::endl;
        return false;
    }

    ENetAddress address;
    enet_address_set_host(&address, hostAddress);
    address.port = port;

    peer = reinterpret_cast<void*>(enet_host_connect(reinterpret_cast<ENetHost*>(host), &address, 2, 0));
    if (peer == nullptr) {
        std::cerr << "Failed to connect to server" << std::endl;
        return false;
    }

    mode = NetworkMode::CLIENT;
    connected = false;
    std::cout << "Connecting to " << hostAddress << ":" << port << std::endl;
    return true;
}

void Network::disconnect() {
    if (peer != nullptr) {
        enet_peer_disconnect(reinterpret_cast<ENetPeer*>(peer), 0);
        peer = nullptr;
    }
    if (host != nullptr) {
        enet_host_destroy(reinterpret_cast<ENetHost*>(host));
        host = nullptr;
    }
    connected = false;
    mode = NetworkMode::NONE;
}

void Network::sendPacket(const std::string& data) {
    sendPacket(data.c_str(), data.length());
}

void Network::sendPacket(const char* data, size_t length) {
    if (host == nullptr) return;

    ENetPacket* packet = enet_packet_create(data, length, ENET_PACKET_FLAG_RELIABLE);

    if (mode == NetworkMode::SERVER && peer != nullptr) {
        enet_peer_send(reinterpret_cast<ENetPeer*>(peer), 0, packet);
    } else if (mode == NetworkMode::CLIENT && host != nullptr) {
        enet_host_broadcast(reinterpret_cast<ENetHost*>(host), 0, packet);
    }
}

void Network::pollEvents() {
    if (host == nullptr) return;

    ENetEvent event;
    while (enet_host_service(reinterpret_cast<ENetHost*>(host), &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                if (mode == NetworkMode::SERVER) {
                    peer = reinterpret_cast<void*>(event.peer);
                    connected = true;
                    std::cout << "Client connected" << std::endl;
                } else if (mode == NetworkMode::CLIENT) {
                    connected = true;
                    std::cout << "Connected to server" << std::endl;
                }
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                if (event.packet->dataLength > 0) {
                    std::string msgData(reinterpret_cast<char*>(event.packet->data), event.packet->dataLength);
                    PacketData packetData;

                    if (msgData.rfind("CLIENT_PADDLE:", 0) == 0) {
                        packetData.type = "CLIENT_PADDLE";
                        std::string coords = msgData.substr(14);
                        size_t commaPos = coords.find(',');
                        if (commaPos != std::string::npos) {
                            packetData.data[0] = std::stof(coords.substr(0, commaPos));
                            packetData.data[1] = std::stof(coords.substr(commaPos + 1));
                        }
                    } else if (msgData.rfind("FULLSTATE", 0) == 0) {
                        packetData.type = "FULLSTATE";
                        packetData.jsonData = msgData;
                    } else {
                        packetData.type = "UNKNOWN";
                        packetData.jsonData = msgData;
                    }

                    receivedPackets.push_back(packetData);
                }
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                if (mode == NetworkMode::SERVER) {
                    connected = false;
                    peer = nullptr;
                    std::cout << "Client disconnected" << std::endl;
                }
                break;
            default:
                break;
        }
    }
}

std::vector<PacketData> Network::getPackets() {
    return receivedPackets;
}

void Network::clearPackets() {
    receivedPackets.clear();
}