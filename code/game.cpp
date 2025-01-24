#include <stdio.h>
#include <stdint.h>
#include "stb_image.h"
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "raylib.h"

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

i32 main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1600, 900, "Divegame");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(WHITE);
        DrawRectangle(100, 100, 100, 100, RED);  
        EndDrawing();
    }
}
