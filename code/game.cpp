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

    Room room = {};
    for (u32 x = 0; x < 20; ++x)
    {
        for (u32 y = 0; y < 15; ++y)
        {
            Tile tile = Tile_Empty;

            if (x >= 3 && x <= 5 && y >= 7 && y <= 8)
            {
                tile = Tile_Wall;
            }

            room.tiles[x + y * ROOM_WIDTH] = tile;

        }
    }

    Room level = load_room();
    SceneMode sceneMode = SCENE_MODE_TEST_DEFAULT;

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_F1))
        {
            sceneMode = SCENE_MODE_TEST_DEFAULT;
        } 
        else if (IsKeyPressed(KEY_F2))
        {
            sceneMode = SCENE_MODE_TEST_PLAYER;
        }

        BeginDrawing();
        ClearBackground(WHITE);

        for (u32 x = 0; x < ROOM_WIDTH; ++x)
        {
            for (u32 y = 0; y < ROOM_HEIGHT; ++y)
            {
                if (room.tiles[x + y * ROOM_WIDTH])
                {
                    DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, RED);  
                }
            }
        }

        switch (sceneMode)
        {
            case SCENE_MODE_TEST_DEFAULT:
                break;
            case SCENE_MODE_TEST_PLAYER:
                test_player_loop();
                break;
        }

        EndDrawing();
    }
}
