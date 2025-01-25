#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "stb_image.h"
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#define lengthof(x) (sizeof(x) / sizeof(x[0]))

#include "essentials.cpp"
#include "renderer.cpp"
#include "player.cpp"
#include "pufferfish.cpp"

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

    // Model model = LoadModel("asset/3d/pufferfish/Pufferfish.glb");
    Model model = LoadModel("asset/3d/toad/Toad.glb");

    RenderTexture room_low = LoadRenderTexture(ROOM_WIDTH * TILE_SIZE_LOW, ROOM_HEIGHT * TILE_SIZE_LOW);

    RenderEntities render_entities = {};
    RenderTexture entities_high = LoadRenderTexture(128 * RENDER_ATLAS_SIZE, 128 * RENDER_ATLAS_SIZE);
    RenderTexture entities_low = LoadRenderTexture(32 * RENDER_ATLAS_SIZE, 32 * RENDER_ATLAS_SIZE);

    Texture2D jason_texture = LoadTexture("asset/jason_texture.png");

    Texture2D wall_texture = LoadTexture("asset/wall_base.png");

    Room level = load_room(0);
    SceneMode sceneMode = SCENE_MODE_TEST_DEFAULT;

    f32 camera_pos_x = TILE_SIZE_HIGH * ROOM_WIDTH / 2;
    f32 camera_pos_y = TILE_SIZE_HIGH * ROOM_HEIGHT / 2;

    Camera camera = { 0 };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.position = {-10, 0, 0};
    camera.target = {0, 0, 0};
    camera.fovy = 5.0f;
    camera.projection = CAMERA_ORTHOGRAPHIC;

    while (!WindowShouldClose())
    {
        state.render_entities = {};

        if (IsKeyPressed(KEY_F1))
        {
            sceneMode = SCENE_MODE_TEST_DEFAULT;
        } 
        else if (IsKeyPressed(KEY_F2))
        {
            sceneMode = SCENE_MODE_TEST_PLAYER;
        }

        // Call render entity here...
        RenderEntity(2, 2);
        RenderEntity(3, 5);

        // Entities to entity buffer
        BeginTextureMode(entities_high);
        ClearBackground(RED);
        BeginMode3D(camera);

        for (u32 i = 0; i < state.render_entities.count; ++i)
        {
            EntityDraw *draw = state.render_entities.entities + i;

            rlViewport(draw->atlas_x * 128, draw->atlas_y * 128, 128, 128);
            DrawModel(model, {}, 1, WHITE);
            DrawCube({0,0}, 1, 1, 1, WHITE);
        }

        EndMode3D();
        EndTextureMode();

        BeginTextureMode(entities_low);
        DrawTexturePro(entities_high.texture, 
                       { 0, 0, (f32)entities_high.texture.width, (f32)-entities_high.texture.height }, 
                       { 0, 0, (f32)entities_low.texture.width, (f32)entities_low.texture.height }, { 0, 0 }, 0, WHITE);
        EndTextureMode();

        // ROOM
        BeginTextureMode(room_low);
        ClearBackground(WHITE);


        //Render Wall
        for (u32 x = 0; x < ROOM_WIDTH; ++x)
        {
            for (u32 y = 0; y < ROOM_HEIGHT; ++y)
            {
                if (level.tiles[x + y * ROOM_WIDTH])
                {
                    //DrawRectangle(x * TILE_SIZE_LOW, y * TILE_SIZE_LOW, TILE_SIZE_LOW, TILE_SIZE_LOW, RED);  
                    
                    DrawTextureRec(wall_texture, {0, 0, 20, 20}, {(f32)TILE_SIZE_LOW * x, (f32)TILE_SIZE_LOW * y}, WHITE);
                }
            }
        }

        // Render entities into room
        for (u32 i = 0; i < state.render_entities.count; ++i)
        {
            EntityDraw *draw = state.render_entities.entities + i; 
            DrawTextureRec(entities_low.texture, {(f32) draw->atlas_x * 32, (f32) draw->atlas_y * 32, 32, 32}, {draw->x * TILE_SIZE_LOW, draw->y * TILE_SIZE_LOW}, WHITE);
        }

        EndTextureMode();

        // Render to swapchain
        BeginDrawing();
        ClearBackground(WHITE);
        DrawTexturePro(room_low.texture, 
                       { 0, 0, (f32)room_low.texture.width, (f32)-room_low.texture.height }, 
                       { 0, 0, (f32)GetRenderWidth(), (f32)GetRenderHeight() }, { 0, 0 }, 0, WHITE);

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
