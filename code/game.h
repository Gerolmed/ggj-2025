#include <stdio.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef int32_t i32;
typedef float f32;

#define ROOM_WIDTH 20
#define ROOM_HEIGHT 10
#define TILE_SIZE 100

enum Tile
{
    Tile_Empty,
    Tile_Wall,
};

struct Room
{
    Tile tiles[ROOM_WIDTH * ROOM_HEIGHT];
};
