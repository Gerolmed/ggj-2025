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

enum Direction
{
    Direction_Up,
    Direction_Down,
    Direction_Left,
    Direction_Right,
};

struct Entrance
{
    bool enabled;
    u32 target_room;
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

Texture texture_ui_heart_full;
Texture texture_ui_heart_half;
Texture texture_ui_heart_empty;
Texture texture_ui_heart_temporary_full;
Texture texture_ui_heart_temporary_half;
struct Health
{
    u32 temp_health;
    u32 health;
    u32 max_health;
    bool dead;
    float damage_indicator;
};

struct Player
{
    Bubble bubbles[6];
    i32 current_bubble;

    Vector2 position;
    f32 rotation;

    
    Health health;

    Vector2 knockback_velocity;

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
    Health health;
};

struct Sharkfish
{
    Vector2 position;
    float rotation;
    Health health;
    Vector2 knockback_velocity;

    i32 behavior_frame;
    Vector2 dash_direction;
};

struct Jellyfish
{
    Vector2 position;
    float size;
    float rotation;
    Health health;
    i32 behavior_frame;
};

struct ProjectileBubble{
    Vector2 position;
    f32 radius;
    Vector2 velocity;
    i32 damage;
    bool can_collide_with_player;
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

    Entrance entrances[4];

    i32 pufferfish_count;
    Pufferfish pufferfishs[64];

    i32 sharkfish_count;
    Sharkfish sharkfishs[16];

    i32 jellyfish_count;
    Jellyfish jellyfishs[16];

    ProjectileBubble *projectiles = NULL;

    ProjectileSpike *spikes = NULL;


};


Music calm_music;
Music dark_music;
Music bubble_sound[3];
Music cry;
Music jump[3];
Music step;

enum ModelType
{
    Model_Toad,
    Model_Fish,
    Model_Spike,
    Model_Bubble,
    Model_Shark,
    Model_Jelly,

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
    Color color;
};

struct RenderEntities
{
    u8 tiles[128];

    u32 count;
    EntityDraw entities[128];
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

    u32 room_count;
    u32 current_room;
    Room rooms[32];

    Player player;


};

GameState state = {};



Color color_from_damage(Health *health)
{
    float t = 1 - health->damage_indicator;
    t = 1 - pow(1 - t, 5);
    return ColorLerp(RED, WHITE, t);
}

void damage(Health *health, u32 amount)
{
    health->damage_indicator = 1;
    if (health->temp_health > 0)
    {
        if (amount > health->temp_health)
        {
            amount -= health->temp_health;
            health->temp_health = 0;
        } else
        {
            health->temp_health -= amount;
            return;
        }
    }

    if (amount > health->health)
    {
        health->health = 0;
        health->dead = true;
    } else
    {
        health->health -= amount;
    }
}

void kill(Health *health)
{
    health->dead = false;
    health->health = 0;
}

void heal(Health *health, const u32 amount)
{
    health->dead = false;
    health->health = fmax(health->health + amount, health->max_health);
}

void grant_temp_health(Health *health, u32 amount)
{
    health->temp_health += amount;
}


void update_health(Health *health)
{
    health->damage_indicator -= GetFrameTime() * 2;
    TraceLog(LOG_INFO, "Damage is %f", health->damage_indicator);
    health->damage_indicator = fmax(health->damage_indicator, 0);
}



f32 fish_get_radius(Pufferfish* fish);
void transition_to_room(Player* player, i32 old_room_id, i32 new_room_id);
