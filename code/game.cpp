#include <stdio.h>
#include <stdint.h>
#include "stb_image.h"
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "game.h"
#include "raylib.h"
#include "player.cpp"

#include "loader.cpp"



enum SceneMode
{
    SCENE_MODE_TEST_DEFAULT,
    SCENE_MODE_TEST_PLAYER,
};

i32 main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1600, 900, "Divegame");
    SetTargetFPS(60);

    Room level = load_room();
    SceneMode sceneMode = SCENE_MODE_TEST_DEFAULT;

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_F1))
        {
            sceneMode = SCENE_MODE_TEST_DEFAULT;
        } else if (IsKeyPressed(KEY_F2))
        {
            sceneMode = SCENE_MODE_TEST_PLAYER;
        }

        BeginDrawing();
        ClearBackground(WHITE);
        switch (sceneMode)
        {
            case SCENE_MODE_TEST_DEFAULT:
                DrawRectangle(100, 100, 100, 100, RED);
                break;
            case SCENE_MODE_TEST_PLAYER:
                test_player_loop();
                break;
        }
        EndDrawing();
    }
}
