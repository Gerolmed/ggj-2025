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

    RenderTexture room_high = LoadRenderTexture(ROOM_WIDTH * TILE_SIZE_HIGH, ROOM_HEIGHT * TILE_SIZE_HIGH);
    RenderTexture room_low = LoadRenderTexture(ROOM_WIDTH * TILE_SIZE_LOW, ROOM_HEIGHT * TILE_SIZE_LOW);

    Texture2D jason_texture = LoadTexture("asset/jason_texture.png");

    Texture2D wall_texture = LoadTexture("asset/wall_base.png");

    Room level = load_room(1);
    SceneMode sceneMode = SCENE_MODE_TEST_DEFAULT;

    Camera camera = { 0 };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.position = {-10, 10, 10};

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

        // Render high texture
        BeginTextureMode(room_high);
        ClearBackground(WHITE);
        for (u32 x = 0; x < ROOM_WIDTH; ++x)
        {
            for (u32 y = 0; y < ROOM_HEIGHT; ++y)
            {
                if (level.tiles[x + y * ROOM_WIDTH])
                {
                    printf("1");
                    //DrawRectangle(x * TILE_SIZE_HIGH, y * TILE_SIZE_HIGH, TILE_SIZE_HIGH, TILE_SIZE_HIGH, RED); 
                    //TODO: Get texture
                    DrawTextureRec(wall_texture, { 0, 0, (f32)20/wall_texture.width, (f32)20/wall_texture.height }, { (float)x, (float)y }, WHITE); 
                }else{
                    printf("0");
                }
            }
            printf("\n");
        }
        //DrawTexture(jason_texture, 0, 0, WHITE);
        EndTextureMode();

        // Render low texture
        BeginTextureMode(room_low);
        DrawTexturePro(room_high.texture, 
                       { 0, 0, (f32)room_high.texture.width, (f32)-room_high.texture.height }, 
                       { 0, 0, (f32)room_low.texture.width, (f32)room_low.texture.height }, { 0, 0 }, 0, WHITE);
        EndTextureMode();

        // Render to swapchain
        BeginDrawing();
        DrawTexturePro(room_low.texture, 
                       { 0, 0, (f32)room_low.texture.width, (f32)-room_low.texture.height }, 
                       { 0, 0, (f32)1600, (f32)880 }, { 0, 0 }, 0, WHITE);

        // BeginMode3D();
        // DrawModel(model, {}, 1, WHITE);
        // EndMode3D();

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
