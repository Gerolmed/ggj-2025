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
#include "collision.cpp"
#include "player.cpp"
#include "pufferfish.cpp"
#include "sharkfish.cpp"

#include "loader.cpp"

enum SceneMode
{
    SCENE_MODE_TEST_DEFAULT,
    SCENE_MODE_TEST_PLAYER,
};

Model models[Model_Count];

Texture2D tileset;

inline void DrawTileAt(u32 tile_x, u32 tile_y, u32 x, u32 y)
{
    DrawTextureRec(tileset, {(f32) tile_x * TILE_SIZE_LOW, (f32) tile_y * TILE_SIZE_LOW, TILE_SIZE_LOW, TILE_SIZE_LOW}, 
                   {(f32) x * TILE_SIZE_LOW, (f32) y * TILE_SIZE_LOW}, WHITE);
}

inline void DrawTileRegion(u32 tile_x, u32 tile_y, u32 x, u32 y, u32 size_x, u32 size_y)
{
    for (u32 dx = 0; dx < size_x; ++dx)
    {
        for (u32 dy = 0; dy < size_y; ++dy)
        {
            DrawTileAt(tile_x + dx, tile_y + dy, x + dx, y + dy);
        }
    }
}

i32 main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1600, 900, "Divegame");
    SetTargetFPS(60);

    // model.transform = model.transform * MatrixTranslate(0,1,1) * MatrixScale(1.0f, 1.0f, 1.0f);
    models[Model_Toad] = LoadModel("asset/3d/toad/Toad.glb");
    models[Model_Toad].transform = models[Model_Toad].transform * MatrixTranslate(0, 1, 0.8) * MatrixScale(1.2f, 1.2f, 1.2f);

    models[Model_Fish] = LoadModel("asset/3d/pufferfish/Pufferfish.glb");
    models[Model_Fish].transform = models[Model_Fish].transform * MatrixTranslate(0, 1, 0.6);

    models[Model_Spike] = LoadModel("asset/3d/pufferfish/spike.glb");
    models[Model_Bubble] = LoadModel("asset/3d/bubble/Bubble.glb");

    models[Model_Shark] = LoadModel("asset/3d/shark/shark.glb");
    models[Model_Shark].transform = models[Model_Shark].transform * MatrixTranslate(0, 1, 0.6);
 
    {
        i32 anim_count;
        ModelAnimation* animation_list = LoadModelAnimations("asset/3d/toad/Toad.glb", &anim_count);

        for (int i = 0; i < anim_count; ++i)
        {
            ModelAnimation* animation = animation_list + i;
            assert(animation);
            if (strcmp(animation->name, "attack_charge") == 0)
            {
                player_model_animations[PlayerAnim_Charge] = *animation;
            }
            else if (strcmp(animation->name, "Idle") == 0)
            {
                player_model_animations[PlayerAnim_Idle] = *animation;
            }
            else if (strcmp(animation->name, "Move") == 0)
            {
                player_model_animations[PlayerAnim_Walk] = *animation;
            }
            else if (strcmp(animation->name, "post_attack") == 0)
            {
                player_model_animations[PlayerAnim_PostShoot] = *animation;
            }
            else
            {
                assert(false);
            }
        }
    }

    Player* player = &state.player;
    configure_player(player);


    RenderTexture room_low = LoadRenderTexture((ROOM_WIDTH + 4) * TILE_SIZE_LOW, (ROOM_HEIGHT + 4) * TILE_SIZE_LOW);

    RenderEntities render_entities = {};
    RenderTexture entities_high = LoadRenderTexture(128 * RENDER_ATLAS_SIZE, 128 * RENDER_ATLAS_SIZE);
    RenderTexture entities_low = LoadRenderTexture(32 * RENDER_ATLAS_SIZE, 32 * RENDER_ATLAS_SIZE);

    tileset = LoadTexture("asset/tileset.png");

    Room* level = &state.room;
    *level = load_room(0);


    SceneMode sceneMode = SCENE_MODE_TEST_DEFAULT;

    Camera model_camera = {0};
    model_camera.up = {0.0f, -1.0f, 0.0f};
    model_camera.position = {-10, 10, 0};
    model_camera.target = {0, 0, 0};
    model_camera.fovy = 5.0f;
    model_camera.projection = CAMERA_ORTHOGRAPHIC;

    main_camera = {};
    main_camera.offset = {0, 0};
    main_camera.rotation = 0;
    main_camera.target = {0, 0};
    main_camera.zoom = GetScreenWidth() / (f32) ((ROOM_WIDTH + 4) * TILE_SIZE_LOW);

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
        // RenderEntity(Model_Fish, {2, 2}, 0);
        // RenderEntity(Model_Fish, {3, 5}, 00);

        execute_player_loop(player, &state);

        for (u32 i = 0; i < level->pufferfish_count; ++i)
        {
            Pufferfish* fish = &level->pufferfishs[i];
            if(fish->dead) continue;
            fish_update(fish, &state);

            RenderEntity(Model_Fish, Vector2(fish->position.x, fish->position.y), 180 + fish->rotation * 180/PI, 2*fish_get_radius(fish));
        }

        for (u32 i = 0; i < level->sharkfish_count; ++i)
        {
            Sharkfish* fish = &level->sharkfishs[i];
            if(fish->dead) continue;
            shark_update(fish, &state);

            RenderEntity(Model_Shark, Vector2(fish->position.x, fish->position.y), 180 + fish->rotation * 180/PI, 1);
        }


        for (u32 i = 0; i < arrlen(level->projectiles); i++)
        {
            ProjectileBubble* projectile = &level->projectiles[i];
            projectile->position.x += GetFrameTime() * projectile->velocity.x;
            projectile->position.y += GetFrameTime() * projectile->velocity.y;

            if(abs_squared(projectile->position) > 10000)
            {
                arrdel(level->projectiles, i);
                i--;
            }

            RenderEntity(Model_Bubble, Vector2(projectile->position.x, projectile->position.y), 0, projectile->radius);
            
        }
        for(u32 i = 0; i < arrlen(level->spikes); i++)
        {
            i32 SPIKE_SPEED = 10;
            ProjectileSpike* spike = &level->spikes[i];
            spike->position.x += GetFrameTime() * SPIKE_SPEED * spike->direction.x;
            spike->position.y += GetFrameTime() * SPIKE_SPEED * spike->direction.y;

            if(abs_squared(spike->position) > 10000)
            {
                arrdel(level->spikes, i);
                i--;
            }
            RenderEntity(Model_Spike, Vector2(spike->position.x, spike->position.y), 0, 1);
            

        }

        // Entities toaaaaaa entity buffer
        BeginTextureMode(entities_high);
        ClearBackground({});

        BeginMode3D(model_camera);

        for (u32 i = 0; i < state.render_entities.count; ++i)
        {
            EntityDraw* draw = state.render_entities.entities + i;

            rlViewport(draw->atlas_x * 128, draw->atlas_y * 128, 128, 128);
            DrawModelEx(models[draw->model], {}, {0,1,0}, draw->rot, {draw->scale,draw->scale,draw->scale}, WHITE);
            // DrawCube({0,0}, 1, 1, 1, WHITE);
        }

        EndMode3D();
        EndTextureMode();


        BeginTextureMode(entities_low);
        ClearBackground({});
        DrawTexturePro(entities_high.texture,
                       {0, 0, (f32)entities_high.texture.width, (f32)-entities_high.texture.height},
                       {0, 0, (f32)entities_low.texture.width, (f32)entities_low.texture.height}, {0, 0}, 0, WHITE);
        EndTextureMode();

        // ROOM
        BeginDrawing();
        BeginMode2D(main_camera);
        ClearBackground(WHITE);


        //Render Wall

        // Topleft
        DrawTileRegion(4, 0, 0, 0, 2, 2);

        // Topright
        DrawTileRegion(8, 0, ROOM_WIDTH + 2, 0, 2, 2);

        // Botleft
        DrawTileRegion(4, 4, 0, ROOM_HEIGHT + 2, 2, 2);

        // Botright
        DrawTileRegion(8, 4, ROOM_WIDTH + 2, ROOM_HEIGHT + 2, 2, 2);

        for (u32 y = 2; y < ROOM_HEIGHT + 1; y += 2)
        {
            DrawTileRegion(4, 2, 0, y, 2, 2);
            DrawTileRegion(8, 2, ROOM_WIDTH + 2, y, 2, 2);
        }
        DrawTileRegion(4, 2, 0, ROOM_HEIGHT + 1, 2, 1);
        DrawTileRegion(8, 2, ROOM_WIDTH + 2, ROOM_HEIGHT + 1, 2, 1);

        for (u32 x = 2; x < ROOM_WIDTH + 1; x += 2)
        {
            DrawTileRegion(6, 0, x, 0, 2, 2);
            DrawTileRegion(6, 4, x, ROOM_HEIGHT + 2, 2, 2);
        }

        // for (u32 x = 0; x < ROOM_WIDTH; ++x)
        // {
        //     for (u32 y = 0; y < ROOM_HEIGHT; ++y)
        //     {
        //         if (level->tiles[x + y * ROOM_WIDTH])
        //         {
        //             //DrawRectangle(x * TILE_SIZE_LOW, y * TILE_SIZE_LOW, TILE_SIZE_LOW, TILE_SIZE_LOW, RED);
        //
        //             DrawTextureRec(tileset, {0, 0, 20, 20}, {(f32)TILE_SIZE_LOW * x, (f32)TILE_SIZE_LOW * y},
        //                            WHITE);
        //         }
        //     }
        // }

        // Render entities into room
        for (u32 i = 0; i < state.render_entities.count; ++i)
        {
            EntityDraw* draw = state.render_entities.entities + i;
            DrawTextureRec(entities_low.texture, {(f32)draw->atlas_x * 32, (f32)draw->atlas_y * 32, 32, 32},
                           {draw->x * TILE_SIZE_LOW, draw->y * TILE_SIZE_LOW}, WHITE);
        }
        printf("%u\n", state.render_entities.count);

        EndMode2D();
        EndDrawing();

    }
}
