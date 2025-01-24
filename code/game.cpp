#include <stdio.h>
#include <stdint.h>

#include "raylib.h"

typedef uint8_t u8;
typedef uint32_t u32;
typedef int32_t i32;

#define ROOM_WIDTH 20
#define ROOM_HEIGHT 15
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

i32 main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1600, 900, "Divegame");
    SetTargetFPS(60);

    Room room = {};
    for (u32 x = 0; x < 20; ++x)
    {
        for (u32 y = 0; y < 15; ++y)
        {
            Tile tile = Tile_Empty;

            if (x >= 3 && x <= 5 && y >= 7 && y <= 8)
            {
                tile = Tile_Wall;
            }

            room.tiles[x + y * ROOM_WIDTH] = tile;

        }
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(WHITE);

        for (u32 x = 0; x < ROOM_WIDTH; ++x)
        {
            for (u32 y = 0; y < ROOM_HEIGHT; ++y)
            {
                if (room.tiles[x + y * ROOM_WIDTH])
                {
                    DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, RED);  
                }
            }
        }

        EndDrawing();
    }
}
