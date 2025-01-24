#include <stdio.h>
#include <stdint.h>


typedef uint8_t u8;
typedef uint32_t u32;
typedef int32_t i32;

enum Tile
{
    Tile_Empty,
    Tile_Wall,
};

struct Room
{
    Tile tiles[20 * 15];
};
