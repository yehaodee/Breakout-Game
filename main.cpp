#include "raylib.h"
#include "Game.h"

int main() {
    InitWindow(game.windowWidth, game.windowHeight, game.windowTitle.c_str());
    Game game;
    game.Init();
    while (!WindowShouldClose()) {
        game.Update();
        game.Draw();
    }
    game.Shutdown();
    CloseWindow();
    return 0;
}