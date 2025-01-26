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
#include "rendering/renderer.cpp"
#include "collision.cpp"

#include "entities/player.cpp"
#include "entities/pufferfish.cpp"
#include "entities/sharkfish.cpp"
#include "entities/jellyfish.cpp"

#include "rendering/room_renderer.cpp"

#include "loader.cpp"


Model models[Model_Count];
Shader skinned_shader;
Shader default_shader;

////////////////////////////////////////////
// Load Music
////////////////////////////////////////////
void LoadMusic(){
    calm_music = LoadMusicStream("asset/sounds/calm_music.wav");
    calm_music.looping = true;
    SetMusicVolume(calm_music,1.0f);
    
    dark_music = LoadMusicStream("asset/sounds/Background_theme.wav");
    dark_music.looping = true;
    SetMusicVolume(calm_music,0.8f);

    bubble_sound[0] = LoadMusicStream("asset/sounds/bubbles_1.wav");
    bubble_sound[0].looping = false;
    bubble_sound[1] = LoadMusicStream("asset/sounds/bubbles_2.wav");
    bubble_sound[1].looping = false;
    bubble_sound[2] = LoadMusicStream("asset/sounds/bubbles_3.wav");
    bubble_sound[2].looping = false;

    cry = LoadMusicStream("asset/sounds/Jump_Dangerous_03.wav");

    jump[0] = LoadMusicStream("asset/sounds/Jump_Friendly_03.wav");
    jump[1] = LoadMusicStream("asset/sounds/Jump_Friendly_05.wav");
    jump[2] = LoadMusicStream("asset/sounds/Jump_Friendly_07.wav");
    step = LoadMusicStream("asset/sounds/Sand_Single_step_12.wav");
}

////////////////////////////////////////////
// Load Shaders
////////////////////////////////////////////
void LoadShaders()
{
    skinned_shader = LoadShader("asset/skinned.vert", "asset/default.frag");
    default_shader = LoadShader("asset/default.vert", "asset/default.frag");

    for (u32 i = 0; i < Model_Count; ++i)
    {
        Shader shader = default_shader;

        if (i == Model_Toad)
        {
            shader = skinned_shader;
        }

        for (u32 j = 0; j < models[i].materialCount; ++j)
        {
            models[i].materials[j].shader = shader;
        }
    }
}


////////////////////////////////////////////
// START EXECUTING PROGRAMM
////////////////////////////////////////////
i32 main()
{
    // Configure device and window
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1600, 900, "Divegame"); 
    InitAudioDevice(); 
    SetTargetFPS(60);

    ////////////////////////////////////////////
    // Load Textures
    ////////////////////////////////////////////
    texture_ui_heart_full = LoadTexture("asset/ui/heart_full.png");
    texture_ui_heart_half = LoadTexture("asset/ui/heart_half.png");
    texture_ui_heart_empty = LoadTexture("asset/ui/heart_empty.png");
    texture_ui_heart_temporary_full = LoadTexture("asset/ui/heart_temporary_full.png");
    texture_ui_heart_temporary_half = LoadTexture("asset/ui/heart_temporary_half.png");


    ////////////////////////////////////////////
    // Load Models
    ////////////////////////////////////////////

    // model.transform = model.transform * MatrixTranslate(0,1,1) * MatrixScale(1.0f, 1.0f, 1.0f);
    models[Model_Toad] = LoadModel("asset/3d/toad/Toad.glb");
    models[Model_Toad].transform = models[Model_Toad].transform * MatrixTranslate(0, 0, 0) * MatrixScale(1.2f, 1.2f, 1.2f);

    models[Model_Fish] = LoadModel("asset/3d/pufferfish/Pufferfish.glb");
    models[Model_Fish].transform = models[Model_Fish].transform * MatrixTranslate(0, 1, 0.6);

    models[Model_Spike] = LoadModel("asset/3d/pufferfish/spike.glb");

    models[Model_Bubble] = LoadModel("asset/3d/bubble/Bubble.glb");

    models[Model_Shark] = LoadModel("asset/3d/shark/shark.glb");
    models[Model_Shark].transform = models[Model_Shark].transform * MatrixTranslate(0, 1, 0.6);

    models[Model_Jelly] = LoadModel("asset/3d/jellyfish/jellyfish.glb");
    models[Model_Jelly].transform = models[Model_Jelly].transform * MatrixTranslate(0, 1, 0.6);

    LoadShaders();
    LoadMusic();

    ////////////////////////////////////////////
    // Prepare player animations
    ////////////////////////////////////////////
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

    RenderEntities render_entities = {};
    RenderTexture entities_high = LoadRenderTexture(128 * RENDER_ATLAS_SIZE, 128 * RENDER_ATLAS_SIZE);
    RenderTexture entities_low = LoadRenderTexture(32 * RENDER_ATLAS_SIZE, 32 * RENDER_ATLAS_SIZE);

    tileset = LoadTexture("asset/tileset.png");

    for (u32 i = 0; i <= 4; ++i)
    {
        assert(state.room_count < lengthof(state.rooms));
        state.rooms[state.room_count++] = load_room(i);
    }

    ////////////////////////////////////////////
    // Configure camera setups
    ////////////////////////////////////////////

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

    ////////////////////////////////////////////
    // Prepare Audio
    ////////////////////////////////////////////
    PlayMusicStream(calm_music);
    PlayMusicStream(dark_music);
    PauseMusicStream(dark_music);

    while (!WindowShouldClose())
    {
        Room *level = state.rooms + state.current_room;
        bool shark_alive = false;
        state.render_entities = {};

        // Configure camera zoom
        main_camera.zoom = GetRenderWidth() / (f32) ((ROOM_WIDTH + 4) * TILE_SIZE_LOW);


        ///////////////////////////////////////////
        // RUN ENTITY MAIN LOOPS. Only allows:
        // RenderEntity(Model_Fish, {2, 2}, 0);
        // for rendering
        ///////////////////////////////////////////
        {
            {
                execute_player_loop(player, &state);

                for (u32 i = 0; i < level->pufferfish_count; ++i)
                {
                    Pufferfish* fish = &level->pufferfishs[i];
                    fish_update(fish, &state);

                }

                for (u32 i = 0; i < level->sharkfish_count; ++i)
                {
                    Sharkfish* fish = &level->sharkfishs[i];
                    shark_update(fish, &state);

                    if(!fish->health.dead) shark_alive = true;
                }

                for (u32 i = 0; i < level->jellyfish_count; ++i)
                {
                    Jellyfish* fish = &level->jellyfishs[i];
                    jellyfish_update(fish, &state);
                }
            }


            ///////////////////////////////////////////
            // Manage Projectiles
            ///////////////////////////////////////////
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

                RenderEntity(Model_Bubble, Vector2(projectile->position.x, projectile->position.y), 0, projectile->radius, WHITE);

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

                RenderEntity(Model_Spike, Vector2(spike->position.x, spike->position.y), 0, 1, WHITE);

            }
        }

        ////////////////////////////////////////////
        // Manage Audio data
        ////////////////////////////////////////////
        {
            UpdateMusicStream(calm_music);
            UpdateMusicStream(dark_music);
            for(i32 i = 0 ; i < 3 ; i++){
                UpdateMusicStream(bubble_sound[i]);
            }

            if(shark_alive){
                ResumeMusicStream(dark_music);
                PauseMusicStream(calm_music);
            }else{
                PauseMusicStream(dark_music);
                ResumeMusicStream(calm_music);
            }
        }

        ////////////////////////////////////////////
        // Begin high-res drawing operations
        ////////////////////////////////////////////
        {
            // Entities to entity buffer
            BeginTextureMode(entities_high);
            ClearBackground({});

            BeginMode3D(model_camera);

            for (u32 i = 0; i < state.render_entities.count; ++i)
            {
                EntityDraw* draw = state.render_entities.entities + i;

                rlViewport(draw->atlas_x * 128, draw->atlas_y * 128, 128, 128);

                if (draw->animation)
                {
                    UpdateModelAnimationBones(models[draw->model], *draw->animation, draw->frame);
                }

                DrawModelEx(models[draw->model], {}, {0,1,0}, draw->rot, {draw->scale,draw->scale,draw->scale}, draw->color);
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
        }


        ////////////////////////////////////////////
        // Begin drawing world
        ////////////////////////////////////////////
        {
            BeginDrawing();
            BeginMode2D(main_camera);
            ClearBackground({69, 54, 34});

            draw_room(level);

            // Render entities into room
            for (u32 i = 0; i < state.render_entities.count; ++i)
            {
                EntityDraw* draw = state.render_entities.entities + i;
                DrawTextureRec(entities_low.texture, {(f32)draw->atlas_x * 32, (f32)draw->atlas_y * 32, 32, 32},
                               {draw->x * TILE_SIZE_LOW, draw->y * TILE_SIZE_LOW}, WHITE);
            }

            EndMode2D();

            draw_player_hud(player);

            EndDrawing();
        }
    }
}
