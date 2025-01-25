#include <stdio.h>
#include <stdint.h>
#include "stb_image.h"
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "raylib.h"




#include "essentials.cpp"
#include "game.h"
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

    Model model = LoadModel("asset/3d/pufferfish/Pufferfish.glb");


    Room level = load_room(1);
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
                if (level.tiles[x + y * ROOM_WIDTH] == Tile_Wall)
                {
                    DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, RED);  
                }
            }
        }

        for(u32 i = 0 ; i < level.transition_tile_count; i++)
        {
            DrawRectangle(level.transition_tiles[i].pos_x * TILE_SIZE, level.transition_tiles[i].pos_y * TILE_SIZE, TILE_SIZE, TILE_SIZE, YELLOW);  
        }

        switch (sceneMode)
        {
            case SCENE_MODE_TEST_DEFAULT:
                break;
            case SCENE_MODE_TEST_PLAYER:
                test_player_loop(nullptr);
                break;
        }

        EndDrawing();
    }
}
