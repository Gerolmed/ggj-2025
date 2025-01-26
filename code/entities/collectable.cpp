void configure_collectable(Collectable* collectable, ItemType type, Vector2 position)
{
    *collectable = {};
    collectable->type = type;
    collectable->position = position;
    collectable->collection_radius = 0.5f;
}

void update_collectables(Room* room, Player* player)
{
    for (int i = 0; i < arrlen(room->collectables); ++i)
    {
        Collectable* collectable = room->collectables + i;

        if (Vector2LengthSqr(player->position - collectable->position) > collectable->collection_radius)
            continue;
        bool collected = false;

        switch (collectable->type)
        {
        case ItemType_Heart_Full:
            if (player->health.health >= player->health.max_health) break;
            if (player->health.health >= player->health.max_health-1)
            {
                heal(&player->health, 1);
                configure_collectable(collectable, ItemType_Heart_Half, collectable->position);
                continue;
            }
            heal(&player->health, 2);
            collected = true;
            break;
        case ItemType_Heart_Half:
            if (player->health.health >= player->health.max_health) break;
            heal(&player->health, 1);
            collected = true;
            break;
        case ItemType_Heart_Temp_Full:
            player->health.temp_health += 2;
            collected = true;
            break;
        case ItemType_Heart_Temp_Half:
            player->health.temp_health += 1;
            collected = true;
            break;
        }

        if (collected)
        {
            arrdel(room->collectables, i);
            i--;
            continue;
        }
    }
}

void draw_collectables(Room* room)
{
    for (int i = 0; i < arrlen(room->collectables); ++i)
    {
        Collectable* collectable = room->collectables + i;
        Texture2D tex = texture_ui_heart_empty;

        switch (collectable->type)
        {
        case ItemType_Heart_Full:
            tex = texture_ui_heart_full;
            break;
        case ItemType_Heart_Half:
            tex = texture_ui_heart_half;
            break;
        case ItemType_Heart_Temp_Full:
            tex = texture_ui_heart_temporary_full;
            break;
        case ItemType_Heart_Temp_Half:
            tex = texture_ui_heart_temporary_half;
            break;
        }


        DrawTexture(tex, collectable->position.x * TILE_SIZE_LOW - tex.width / 2,
                    collectable->position.y * TILE_SIZE_LOW - tex.height / 2, WHITE);
    }
}
