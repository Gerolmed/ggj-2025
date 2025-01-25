#include <stdio.h>
#include <stdint.h>
#include "stb_image.h"
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "raylib.h"
#include "rlgl.h"

#include "essentials.cpp"
#include "game.h"
#include "player.cpp"

#include "loader.cpp"

enum SceneMode
{
    SCENE_MODE_TEST_DEFAULT,
    SCENE_MODE_TEST_PLAYER,
};

struct RenderEntities
{
    u32 size;
    u8 used[128];
};

void AllocateRenderEntity()
{
}

i32 main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1600, 900, "Divegame");
    SetTargetFPS(60);

    Model model = LoadModel("asset/3d/pufferfish/Pufferfish.glb");
    // Model model = LoadModel("asset/3d/toad/Toad.glb");

    RenderTexture room_low = LoadRenderTexture(ROOM_WIDTH * TILE_SIZE_LOW, ROOM_HEIGHT * TILE_SIZE_LOW);

    RenderEntities render_entities = {};
    render_entities.size = 8;
    RenderTexture entities_high = LoadRenderTexture(128 * render_entities.size, 128 * render_entities.size);
    RenderTexture entities_low = LoadRenderTexture(32 * render_entities.size, 32 * render_entities.size);

    Texture2D jason_texture = LoadTexture("asset/jason_texture.png");

    Texture2D wall_texture = LoadTexture("asset/wall_base.png");

    Room level = load_room(1);
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
        if (IsKeyPressed(KEY_F1))
        {
            sceneMode = SCENE_MODE_TEST_DEFAULT;
        } 
        else if (IsKeyPressed(KEY_F2))
        {
            sceneMode = SCENE_MODE_TEST_PLAYER;
        }

        // Entities
        BeginTextureMode(entities_high);
        rlViewport(0, 0, 128, 128);
        ClearBackground(WHITE);
        BeginMode3D(camera);
        DrawModel(model, {}, 1, WHITE);
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
        // for (u32 x = 0; x < ROOM_WIDTH; ++x)
        // {
        //     for (u32 y = 0; y < ROOM_HEIGHT; ++y)
        //     {
        //         if (room.tiles[x + y * ROOM_WIDTH])
        //         {
        //             DrawRectangle(x * TILE_SIZE_HIGH, y * TILE_SIZE_HIGH, TILE_SIZE_HIGH, TILE_SIZE_HIGH, RED);  
        //         }
        //     }
        // }
        // DrawTexture(jason_texture, 0, 0, WHITE);
        EndTextureMode();

        // Render to swapchain
        BeginDrawing();
        DrawTexturePro(room_low.texture, 
                       { 0, 0, (f32)room_low.texture.width, (f32)-room_low.texture.height }, 
                       { 0, 0, (f32)1600, (f32)880 }, { 0, 0 }, 0, WHITE);

        DrawTextureRec(entities_high.texture, {0, 0, 128, 128}, {0, 0}, WHITE);

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
