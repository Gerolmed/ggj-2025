
void configure_collectable(Collectable *collectable, ItemType type)
{
    collectable->type = type;
}

void update_collectables(Room *room, Player *player) {

}
void draw_collectables(Room *room) {
    for (int i = 0; i < arrlen(room->collectables); ++i)
    {
        Collectable *collectable = room->collectables + i;
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


        DrawTexture(tex, collectable->position.x * TILE_SIZE_LOW - tex.width / 2, collectable->position.y * TILE_SIZE_LOW - tex.height / 2, WHITE);
    }
}