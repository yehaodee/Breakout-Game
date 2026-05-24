#include "GameSingle.h"

const std::string SAVE_PATH = "save.json";

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
        if (HasSaveFile(SAVE_PATH) && IsKeyPressed(KEY_R)) {
            if (LoadGame(SAVE_PATH)) {
                currentState = PLAYING;
            }
        }
        break;
    }
    case PLAYING: {
        if (IsKeyPressed(KEY_S)) {
            SaveGame(SAVE_PATH);
        }
        UpdatePlayingState();
        break;
    }
    case PAUSED: {
        if (IsKeyPressed(KEY_P)) {
            currentState = PLAYING;
        }
        if (IsKeyPressed(KEY_S)) {
            SaveGame(SAVE_PATH);
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