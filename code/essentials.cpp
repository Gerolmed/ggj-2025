///////////////////////////////////////////
// Data Type Definitions
///////////////////////////////////////////

typedef uint8_t u8;
typedef uint32_t u32;
typedef int32_t i32;
typedef float f32;

#define ROOM_WIDTH 16
#define ROOM_HEIGHT 7
#define TILE_SIZE_HIGH 80
#define TILE_SIZE_LOW 20
#define RENDER_ATLAS_SIZE 8
#define RENDER_HIGH_CELL_SIZE 256.0f
#define RENDER_LOW_CELL_SIZE (RENDER_HIGH_CELL_SIZE / 4.0f)

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

    bool upgraded;
};

struct Jellyfish
{
    Vector2 position;
    float size;
    float rotation;
    Health health;
    i32 behavior_frame;
};

struct ProjectileBubble
{
    Vector2 position;
    f32 radius;
    Vector2 velocity;
    i32 damage;
    bool can_collide_with_player;
};

f32 SPIKE_RADIUS = 0.1;

struct ProjectileSpike
{
    Vector2 position;
    Vector2 direction;
};

struct Room
{
    i32 id;

    Tile tiles[(ROOM_WIDTH + 4) * (ROOM_HEIGHT + 4)];

    Entrance entrances[4];

    i32 pufferfish_count;
    Pufferfish pufferfishs[64];

    i32 sharkfish_count;
    Sharkfish sharkfishs[16];

    i32 jellyfish_count;
    Jellyfish jellyfishs[16];

    ProjectileBubble* projectiles = NULL;

    ProjectileSpike* spikes = NULL;
};

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

    ModelAnimation* animation;
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

///////////////////////////////////////////
// Global Data
///////////////////////////////////////////

Camera2D main_camera;
ModelAnimation player_model_animations[4] = {};

Texture texture_ui_heart_full;
Texture texture_ui_heart_half;
Texture texture_ui_heart_empty;
Texture texture_ui_heart_temporary_full;
Texture texture_ui_heart_temporary_half;

Texture2D tileset;

Music calm_music;
Music dark_music;
Music bubble_sound[3];
Music cry;
Music jump[3];
Music step;

GameState state = {};


///////////////////////////////////////////
// Utility Functions
///////////////////////////////////////////
Color color_from_damage(Health* health)
{
    float t = 1 - health->damage_indicator;
    t = 1 - pow(1 - t, 5);
    return ColorLerp(RED, WHITE, t);
}

void damage(Health* health, u32 amount)
{
    // grant invulnerability if damage indicator still playing
    if (health->damage_indicator > 0) return;

    health->damage_indicator = 1;
    if (health->temp_health > 0)
    {
        if (amount > health->temp_health)
        {
            amount -= health->temp_health;
            health->temp_health = 0;
        }
        else
        {
            health->temp_health -= amount;
            return;
        }
    }

    if (amount >= health->health)
    {
        health->health = 0;
        health->dead = true;
    }
    else
    {
        health->health -= amount;
    }
}

void kill(Health* health)
{
    health->dead = true;
    health->health = 0;
}

void heal(Health* health, const u32 amount)
{
    health->dead = false;
    health->health = fmax(health->health + amount, health->max_health);
}

void grant_temp_health(Health* health, u32 amount)
{
    health->temp_health += amount;
}


void update_health(Health* health)
{
    health->damage_indicator -= GetFrameTime() * 2;
    health->damage_indicator = fmax(health->damage_indicator, 0);
}

Tile get_safe_tile(Room* room, i32 x, i32 y)
{
    if (x < 0 || y < 0) return Tile_Wall;
    if (ROOM_WIDTH + 4 <= x || ROOM_HEIGHT + 4 <= y) return Tile_Wall;
    return room->tiles[y * (ROOM_WIDTH + 4) + x];
}

bool collide_with_room(Room* room, Vector2 new_pos, Vector2 old_pos, Vector2 *out_pos)
{
    *out_pos = new_pos;
    i32 new_pos_x = new_pos.x;
    i32 new_pos_y = new_pos.y;
    i32 old_pos_x = old_pos.x;
    i32 old_pos_y = old_pos.y;
    bool collided = false;

    if (old_pos_x != new_pos_x && get_safe_tile(room, new_pos_x, old_pos_y) != Tile_Empty)
    {
        collided = true;
        // Collision direction right?
        if (new_pos_x > old_pos_x)
        {
            out_pos->x = new_pos_x - 0.001f;
        }
        else
        {
            out_pos->x = old_pos_x;
        }
    }
    if (old_pos_y != new_pos_y && get_safe_tile(room, old_pos_x, new_pos_y) != Tile_Empty)
    {
        collided = true;
        // Collision direction right?
        if (new_pos_y > old_pos_y)
        {
            out_pos->y = new_pos_y - 0.001f;
        }
        else
        {
            out_pos->y = old_pos_y;
        }
    }

    return collided;
}

f32 fish_get_radius(Pufferfish* fish);
void transition_to_room(Player* player, i32 old_room_id, i32 new_room_id);
