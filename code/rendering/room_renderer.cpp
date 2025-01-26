
inline void DrawTileAt(u32 tile_x, u32 tile_y, u32 x, u32 y)
{
    DrawTextureRec(tileset, {(f32) tile_x * TILE_SIZE_LOW, (f32) tile_y * TILE_SIZE_LOW, TILE_SIZE_LOW, TILE_SIZE_LOW},
                   {(f32) x * TILE_SIZE_LOW, (f32) y * TILE_SIZE_LOW}, WHITE);
}

inline void DrawTileRegion(u32 tile_x, u32 tile_y, u32 x, u32 y, u32 size_x, u32 size_y)
{
    for (u32 dx = 0; dx < size_x; ++dx)
    {
        for (u32 dy = 0; dy < size_y; ++dy)
        {
            DrawTileAt(tile_x + dx, tile_y + dy, x + dx, y + dy);
        }
    }
}

void draw_room(Room *level) {
    // Render floor
    for (u32 x = 3; x < ROOM_WIDTH + 1; ++x)
    {
        for (u32 y = 3; y < ROOM_HEIGHT + 1; ++y)
        {
            DrawTileAt(8, 7, x, y);
        }
    }

    level->entrances[Direction_Left].enabled = true;
    level->entrances[Direction_Right].enabled = true;
    level->entrances[Direction_Up].enabled = true;
    level->entrances[Direction_Down].enabled = true;

    // Floor corners
    DrawTileAt(7, 6, 2, 2);
    DrawTileAt(9, 6, ROOM_WIDTH + 1, 2);
    DrawTileAt(7, 8, 2, ROOM_HEIGHT + 1);
    DrawTileAt(9, 8, ROOM_WIDTH + 1, ROOM_HEIGHT + 1);

    // Floor edges
    for (u32 x = 3; x < ROOM_WIDTH + 1; ++x)
    {
        DrawTileAt(8, 9, x, 2);
        DrawTileAt(8, 10, x, ROOM_HEIGHT + 1);
    }

    for (u32 y = 3; y < ROOM_HEIGHT + 1; ++y)
    {
        DrawTileAt(2, 5, 2, y);
        DrawTileAt(3, 5, ROOM_WIDTH + 1, y);
    }

    if (level->entrances[Direction_Left].enabled)
    {
        DrawTileAt(7, 7, 2, 5);
        DrawTileAt(3, 8, 1, 5);
        DrawTileAt(3, 9, 0, 5);
    }

    if (level->entrances[Direction_Right].enabled)
    {
        DrawTileAt(9, 7, ROOM_WIDTH + 1, 5);
        DrawTileAt(3, 8, ROOM_WIDTH + 2, 5);
        DrawTileAt(3, 9, ROOM_WIDTH + 3, 5);
    }

    if (level->entrances[Direction_Up].enabled)
    {
        DrawTileAt(5, 11, 9, 2);
        DrawTileAt(5, 10, 10, 2);

        DrawTileAt(2, 5, 9, 1);
        DrawTileAt(3, 5, 10, 1);
        DrawTileAt(2, 5, 9, 0);
        DrawTileAt(3, 5, 10, 0);
    }

    if (level->entrances[Direction_Down].enabled)
    {
        DrawTileAt(2, 10, 9, ROOM_HEIGHT + 1);
        DrawTileAt(3, 10, 10, ROOM_HEIGHT + 1);

        DrawTileAt(2, 5, 9, ROOM_HEIGHT + 2);
        DrawTileAt(3, 5, 10, ROOM_HEIGHT + 2);
        DrawTileAt(2, 5, 9, ROOM_HEIGHT + 3);
        DrawTileAt(3, 5, 10, ROOM_HEIGHT + 3);
    }
    
    // Draw floor pattern
    {
        u32 x = 8;
        u32 y = 3;

        DrawTileAt(5 + 1, 6, x, y);
        DrawTileAt(5, 6, x + 1, y);
        DrawTileAt(5, 6 + 2, x + 2, y);
        DrawTileAt(5, 6, x + 3, y);

        DrawTileAt(5 + 1, 6, x + 2, y + 1);
        DrawTileAt(5, 6, x + 3, y + 1);

        DrawTileAt(5 + 1, 6 + 2, x + 1, y + 2);
        DrawTileAt(5, 6, x + 2, y + 2);

        DrawTileAt(5 + 1, 6 + 3, x + 1, y + 3);
        DrawTileAt(5, 6, x + 2, y + 3);

        DrawTileAt(5, 6, x + 1, y + 4);
        DrawTileAt(5 + 1, 6 + 1, x + 2, y + 4);

        // x = 13;
        // DrawTileAt(5, 6, x, y);
        // DrawTileAt(5, 6, x + 1, y);
        // DrawTileAt(5, 6, x + 1, y + 1);
    }

    // Draw gradients
    if (level->entrances[Direction_Left].enabled)
    {
        DrawTexturePro(gradient, {0, 0, 40, 1}, {4 * TILE_SIZE_LOW, 6 * TILE_SIZE_LOW, 2 * TILE_SIZE_LOW, TILE_SIZE_LOW}, 
                       {0.5, 0.5}, 180, WHITE);
    }

    // Render Wall

    // Topleft
    DrawTileRegion(4, 0, 0, 0, 2, 2);

    // Topright
    DrawTileRegion(8, 0, ROOM_WIDTH + 2, 0, 2, 2);

    // Botleft
    DrawTileRegion(4, 4, 0, ROOM_HEIGHT + 2, 2, 2);

    // Botright
    DrawTileRegion(8, 4, ROOM_WIDTH + 2, ROOM_HEIGHT + 2, 2, 2);

    u32 last;

    // level->entrances[Direction_Left].enabled = true;
    // level->entrances[Direction_Right].enabled = true;
    // level->entrances[Direction_Up].enabled = true;
    // level->entrances[Direction_Down].enabled = true;

    if (level->entrances[Direction_Left].enabled)
    {
        DrawTileRegion(4, 2, 0, 2, 2, 1);
        DrawTileRegion(2, 2, 0, 3, 2, 2);

        DrawTileRegion(2, 0, 0, 6, 2, 2);
        DrawTileRegion(4, 2, 0, 8, 2, 1);
    }
    else
{
        last = 0;
        for (u32 y = 2; y < ROOM_HEIGHT + 2; ++y)
        {
            DrawTileRegion(4, 2 + last, 0, y, 2, 1);
            last = (last + 1) % 2;
        }
    }

    if (level->entrances[Direction_Right].enabled)
    {
        DrawTileRegion(8, 2, ROOM_WIDTH + 2, 2, 2, 1);
        DrawTileRegion(0, 2, ROOM_WIDTH + 2, 3, 2, 2);

        DrawTileRegion(0, 0, ROOM_WIDTH + 2, 6, 2, 2);
        DrawTileRegion(8, 2, ROOM_WIDTH + 2, 8, 2, 1);
    }
    else
{
        last = 0;
        for (u32 y = 2; y < ROOM_HEIGHT + 2; ++y)
        {
            DrawTileRegion(8, 2 + last, ROOM_WIDTH + 2, y, 2, 1);
            last = (last + 1) % 2;
        }
    }

    if (level->entrances[Direction_Up].enabled)
    {
        DrawTileRegion(6, 0, 2, 0, 2, 2);
        DrawTileRegion(6, 0, 4, 0, 2, 2);
        DrawTileRegion(6, 0, 6, 0, 2, 2);

        DrawTileRegion(1, 4, 8, 0, 1, 1);
        DrawTileRegion(1, 7, 8, 1, 1, 1);

        DrawTileRegion(0, 4, 11, 0, 1, 1);
        DrawTileRegion(0, 7, 11, 1, 1, 1);

        DrawTileRegion(6, 0, 12, 0, 2, 2);
        DrawTileRegion(6, 0, 14, 0, 2, 2);
        DrawTileRegion(6, 0, 16, 0, 2, 2);
    }
    else
{
        last = 0;
        for (u32 x = 2; x < ROOM_WIDTH + 2; ++x)
        {
            DrawTileRegion(6 + last, 0, x, 0, 1, 2);
            last = (last + 1) % 2;
        }
    }

    if (level->entrances[Direction_Down].enabled)
    {
        DrawTileRegion(6, 4, 2, ROOM_HEIGHT + 2, 2, 2);
        DrawTileRegion(6, 4, 4, ROOM_HEIGHT + 2, 2, 2);
        DrawTileRegion(6, 4, 6, ROOM_HEIGHT + 2, 2, 2);

        DrawTileRegion(1, 6, 8, ROOM_HEIGHT + 2, 1, 1);
        DrawTileRegion(1, 5, 8, ROOM_HEIGHT + 3, 1, 1);

        DrawTileRegion(0, 6, 11, ROOM_HEIGHT + 2, 1, 1);
        DrawTileRegion(0, 5, 11, ROOM_HEIGHT + 3, 1, 1);

        DrawTileRegion(6, 4, 12, ROOM_HEIGHT + 2, 2, 2);
        DrawTileRegion(6, 4, 14, ROOM_HEIGHT + 2, 2, 2);
        DrawTileRegion(6, 4, 16, ROOM_HEIGHT + 2, 2, 2);
    }
    else
{
        last = 0;
        for (u32 x = 2; x < ROOM_WIDTH + 2; ++x)
        {
            DrawTileRegion(6 + last, 4, x, ROOM_HEIGHT + 2, 1, 2);
            last = (last + 1) % 2;
        }
    }
}
