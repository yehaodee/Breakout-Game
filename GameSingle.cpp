#include "GameSingle.h"

GameSingle::GameSingle() : Game() {
    gameMode = SINGLE_PLAYER;
    currentState = MENU;
}

GameSingle::~GameSingle() {
}

void GameSingle::Init() {
    Game::Init();
}

void GameSingle::Update() {
    switch (currentState) {
    case MENU: {
        if (IsKeyPressed(KEY_ENTER)) {
            ResetGame();
            currentState = PLAYING;
        }
        break;
    }
    case PLAYING: {
        UpdatePlayingState();
        break;
    }
    case PAUSED: {
        if (IsKeyPressed(KEY_P)) {
            currentState = PLAYING;
        }
        break;
    }
    case GAME_OVER:
    case VICTORY: {
        if (IsKeyPressed(KEY_ENTER)) {
            currentState = MENU;
        }
        break;
    }
    }
}