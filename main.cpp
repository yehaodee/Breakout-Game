#include "raylib.h"
#include "Game.h"
#include "GameServer.h"
#include "GameClient.h"
#include "GameSingle.h"
#include <cstring>

int main(int argc, char* argv[]) {
    // 检查命令行参数
    const char* mode = nullptr;
    if (argc > 1) {
        mode = argv[1];
    }

    if (mode && strcmp(mode, "server") == 0) {
        GameServer game;
        game.Init();
        while (!game.ShouldClose()) {
            game.Update();
            game.Draw();
        }
        game.Close();
    } else if (mode && strcmp(mode, "client") == 0) {
        GameClient game;
        game.Init();
        while (!game.ShouldClose()) {
            game.Update();
            game.Draw();
        }
        game.Close();
    } else {
        GameSingle game;
        game.Init();
        while (!game.ShouldClose()) {
            game.Update();
            game.Draw();
        }
        game.Shutdown();
    }
    
    CloseWindow();
    return 0;
}