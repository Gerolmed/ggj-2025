#include <math.h>

Camera2D main_camera;

typedef uint8_t u8;
typedef uint32_t u32;
typedef int32_t i32;
typedef float f32;

#define ROOM_WIDTH 16
#define ROOM_HEIGHT 7
#define TILE_SIZE_HIGH 80
#define TILE_SIZE_LOW 20
#define RENDER_ATLAS_SIZE 8

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

    i32 health;

    bool charging;

    // value 0-1 incl
    f32 charge_value;
    f32 last_shot_age;

    PlayerAnim animation;
    i32 frame;

    f32 speed;
    f32 fire_delay;

    Model model;
};

struct Pufferfish
{
    Vector2 position;
    float rotation;
    i32 health;
    bool dead;
};

struct ProjectileBubble{
    Vector2 position;
    f32 radius;
    Vector2 velocity;
    i32 damage;
};

f32 SPIKE_RADIUS = 0.1;
struct ProjectileSpike{
    Vector2 position;
    Vector2 direction;
};

struct Room
{
    i32 id;

    Tile tiles[ROOM_WIDTH * ROOM_HEIGHT];

    i32 transition_tile_count;
    TransitionTile transition_tiles[64];

    i32 pufferfish_count;
    Pufferfish pufferfishs[64];

    ProjectileBubble *projectiles = NULL;

    ProjectileSpike *spikes = NULL;


};

enum ModelType
{
    Model_Toad,
    Model_Fish,
    Model_Spike,
    Model_Bubble,

    Model_Count
};

struct EntityDraw
{
    u32 atlas_x;
    u32 atlas_y;
    
    ModelType model;

    f32 x;
    f32 y;
    f32 rot;
    f32 scale;

    ModelAnimation *animation;
    u32 frame;
};

struct RenderEntities
{
    u8 tiles[128];

    u32 count;
    EntityDraw entities[32];
};

struct AABB
{
    Vector2 position;
    Vector2 size;
};

struct SphericalCollider
{
    Vector2 position;
    f32 radius;
};

struct GameState
{
    RenderEntities render_entities;

    Room room;

    Player player;


};

GameState state = {};






f32 fish_get_radius(Pufferfish* fish);
