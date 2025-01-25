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
#include "player.cpp"
#include "pufferfish.cpp"

#include "loader.cpp"

enum SceneMode
{
    SCENE_MODE_TEST_DEFAULT,
    SCENE_MODE_TEST_PLAYER,
};

GameState state = {};

void RenderEntity()
{
    assert(state.render_entities.entity_count < lengthof(state.render_entities.entities));

    EntityDraw *draw = state.render_entities.entities + state.render_entities.entity_count++;
    memset(draw, 0, sizeof(*draw));
}

i32 main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1600, 900, "Divegame");
    SetTargetFPS(60);

    // Model model = LoadModel("asset/3d/pufferfish/Pufferfish.glb");
    // model.transform = model.transform * MatrixTranslate(0,1,1) * MatrixScale(1.0f, 1.0f, 1.0f);
    Model model = LoadModel("asset/3d/toad/Toad.glb");
    model.transform = model.transform * MatrixTranslate(0,1,0.8) * MatrixScale(1.2f, 1.2f, 1.2f);

    RenderTexture room_low = LoadRenderTexture(ROOM_WIDTH * TILE_SIZE_LOW, ROOM_HEIGHT * TILE_SIZE_LOW);

    RenderEntities render_entities = {};
    render_entities.size = 8;
    RenderTexture entities_high = LoadRenderTexture(128 * render_entities.size, 128 * render_entities.size);
    RenderTexture entities_low = LoadRenderTexture(32 * render_entities.size, 32 * render_entities.size);

    Texture2D jason_texture = LoadTexture("asset/jason_texture.png");

    Texture2D wall_texture = LoadTexture("asset/wall_base.png");

    Room level = load_room(0);
    SceneMode sceneMode = SCENE_MODE_TEST_DEFAULT;

    f32 camera_pos_x = TILE_SIZE_HIGH * ROOM_WIDTH / 2;
    f32 camera_pos_y = TILE_SIZE_HIGH * ROOM_HEIGHT / 2;

    Camera model_camera = { 0 };
    model_camera.up = { 0.0f, -1.0f, 0.0f };
    model_camera.position = {-10, 10, 0};
    model_camera.target = {0, 0, 0};
    model_camera.fovy = 5.0f;
    model_camera.projection = CAMERA_ORTHOGRAPHIC;

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
        ClearBackground(PINK);
        BeginMode3D(model_camera);
        DrawModelEx(model, {}, {0,1,0}, GetTime() * 360, {1,1,1}, WHITE);
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

        //Render Pufferfish
        for(u32 i = 0 ; i < level.pufferfish_count; i++)
        {
            Pufferfish* fish = &level.pufferfishs[i];
            f32 radius = fish_get_radius(fish);
            DrawRectangle(fish->position.x * TILE_SIZE_LOW - radius, fish->position.y * TILE_SIZE_LOW - radius, 2*radius, 2*radius, RED);
        }


        DrawTextureRec(entities_low.texture, {0, 0, 32, 32}, {0, 0}, WHITE);

        EndTextureMode();

        // Render to swapchain
        BeginDrawing();
        ClearBackground(WHITE);
        DrawTexturePro(room_low.texture, 
                       { 0, 0, (f32)room_low.texture.width, (f32)-room_low.texture.height }, 
                       { 0, 0, (f32)1600, (f32)880 }, { 0, 0 }, 0, WHITE);

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
