#ifndef GAMECLIENT_H
#define GAMECLIENT_H

#include "Game.h"
#include "Network.h"

class GameClient : public Game {
private:
    Network network;
    bool connected;

public:
    GameClient();
    ~GameClient();

    void Init();
    void Update() override;
    void Draw();
    bool ShouldClose() const;
    void Close();

private:
    void handleServerPackets();
    void sendPaddlePosition();
};

#endif