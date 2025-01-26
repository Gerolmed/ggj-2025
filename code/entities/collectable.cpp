void configure_collectable(Collectable* collectable, ItemType type, Vector2 position)
{
    *collectable = {};
    collectable->type = type;
    collectable->position = position;
    collectable->collection_radius = 0.6f;
    collectable->push_radius = 0.8f;

    collectable->bobbing_speed = 1;
}

void update_collectables(Room* room, Player* player)
{
    for (int i = 0; i < arrlen(room->collectables); ++i)
    {
        Collectable* collectable = room->collectables + i;

        if (Vector2LengthSqr(player->position - collectable->position) > collectable->collection_radius * collectable->collection_radius)
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

        if (Vector2LengthSqr(player->position - collectable->position) > collectable->push_radius * collectable->push_radius)
            continue;

        Vector2 player_dir = Vector2Normalize(player->last_delta_pos);
        if (Vector2LengthSqr(player_dir) == 0) continue;

        Vector2 collectable_dir = Vector2Normalize(collectable->position - player->position);

        f32 influence;
        f32 factor = 0.05f;
        if (fabs(Vector2Angle(player_dir, collectable_dir)) > 90)
        {
            influence = 0;
            factor = 0.02f;
        }
        else influence = Vector2DotProduct(player_dir, collectable_dir);

        Vector2 old_pos = collectable->position;
        collectable->position += Vector2Lerp(collectable_dir, player_dir, influence) * factor;

        collide_with_room(room, collectable->position, old_pos, &collectable->position);
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

        Vector2 position = collectable->position * TILE_SIZE_LOW;

        collectable->bobbing_state += GetFrameTime() * collectable->bobbing_speed;
        if (collectable->bobbing_state > 2) collectable->bobbing_state -= 2;

        float height_value = 0;

        if (collectable->bobbing_state < 1)
        {
            height_value = easeInOutQuad(collectable->bobbing_state);
        } else
        {
            height_value = easeInOutQuad(1 - (collectable->bobbing_state - 1));
        }

        Texture2D drop_texture;
        if (height_value < .33) drop_texture = drop_shadow[ShadowSize_Smaller];
        else if (height_value < .66) drop_texture = drop_shadow[ShadowSize_Tiny];
        else drop_texture = drop_shadow[ShadowSize_Micro];


        DrawTexture(drop_texture, position.x - drop_texture.width / 2,
                    position.y - drop_texture.height / 2, WHITE);

        DrawTexture(tex, position.x - tex.width / 2,
                    position.y - tex.height / 2 - height_value * 5.0f, WHITE);
    }
}
