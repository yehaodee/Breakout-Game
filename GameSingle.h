#ifndef GAMESINGLE_H
#define GAMESINGLE_H

#include "Game.h"

class GameSingle : public Game {
public:
    GameSingle();
    ~GameSingle();

    void Init();
    void Update() override;
};

#endif