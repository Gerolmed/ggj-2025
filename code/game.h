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

struct Room
{
    Tile tiles[ROOM_WIDTH * ROOM_HEIGHT];

    i32 transition_tile_count;
    TransitionTile transition_tiles[64];

};
