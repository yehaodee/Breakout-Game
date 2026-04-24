#ifndef GAMESERVER_H
#define GAMESERVER_H

#include "Game.h"
#include "Network.h"

class GameServer : public Game {
private:
    Network network;

public:
    GameServer();
    ~GameServer();

    void Init();
    void Update() override;
    void Draw();
    bool ShouldClose() const;
    void Close();

private:
    void handleClientPackets();
    void sendGameState();
};

#endif