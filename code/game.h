#include <stdio.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef int32_t i32;
typedef float f32;

#define ROOM_WIDTH 20
#define ROOM_HEIGHT 11
#define TILE_SIZE_HIGH 80
#define TILE_SIZE_LOW 20

enum Tile
{
    Tile_Empty,
    Tile_Wall,
};

struct TransitionTile
{
    i32 pos_x;
    i32 pos_y;
    i32 new_room_id;
};

struct Bubble
{
    // unscaled radius (of model)
    float radius;
    float min_scale;
    float max_scale;
};

enum PlayerAnim
{
    PlayerAnim_Idle,
    PlayerAnim_Walk,
    PlayerAnim_Charge,
    PlayerAnim_PostShoot,
};

ModelAnimation player_model_animations[4] = {};

struct Player
{
    Bubble bubbles[6];
    i32 current_bubble;

    Vector2 position;
    f32 rotation;

    bool charging;

    // value 0-1 incl
    f32 charge_value;
    f32 last_shot_age;

    PlayerAnim animation;
    i32 frame;

    f32 speed;
    f32 fire_delay;
};

struct Pufferfish
{
    Vector2 position;
    i32 health;
};

struct Room
{
    i32 id;

    Tile tiles[ROOM_WIDTH * ROOM_HEIGHT];

    i32 transition_tile_count;
    TransitionTile transition_tiles[64];

    i32 pufferfish_count;
    Pufferfish pufferfishs[64];



};

struct EntityDraw
{
    u32 x;
    u32 y;
    u32 width;
    u32 height;
};

struct RenderEntities
{
    u32 size;
    u8 used[128];

    u32 entity_count;
    EntityDraw entities[32];
};

struct GameState
{
    RenderEntities render_entities;
};
