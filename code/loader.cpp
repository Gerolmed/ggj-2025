Room load_room(i32 room_id){
    Room room = {};

    room.id = room_id;

    i32 width;
    i32 height;

    char path[1024];
    sprintf(path, "asset/rooms/%d.png", room_id);
    u8* tmp = stbi_load(path, (i32*) &width, (i32*) &height, NULL, STBI_rgb);
    assert(tmp);

    assert(ROOM_WIDTH == width);
    assert(ROOM_HEIGHT == height);

    u8* curr = tmp;
    for (u32 y = 0; y < ROOM_HEIGHT; ++y) {
        for (u32 x = 0; x < ROOM_WIDTH; ++x) {
            if(curr[0] == 0 && curr[1] == 0 && curr[2] == 0){
                room.tiles[ROOM_WIDTH * y + x] = Tile_Wall;
            }
            else if(curr[0] == 255 && curr[1] == 255 && curr[2] < 64){
                TransitionTile* transition_tile = &room.transition_tiles[room.transition_tile_count];
                transition_tile->pos_x = x;
                transition_tile->pos_y = y;
                transition_tile->new_room_id = curr[2];
                room.transition_tile_count++;
            }
            else if(curr[0] == 255 && curr[1] == 0 & curr[2] == 0){
                Pufferfish* pufferfish = &room.pufferfishs[room.pufferfish_count];
                pufferfish->position = Vector2(x, y);
                pufferfish->health = 16;
                room.pufferfish_count++;
            }
            else if(curr[0] == 100 && curr[1] == 100 && curr[2] == 100){
                Sharkfish* sharkfish = &room.sharkfishs[room.sharkfish_count];
                sharkfish->position = Vector2(x,y);
                sharkfish->health = 3;
                room.sharkfish_count++;
            }
            else if(curr[0] == 0 && curr[1] == 255 && curr[2] == 255){
                Jellyfish* jellyfish = &room.jellyfishs[room.jellyfish_count];
                jellyfish->position = Vector2(x,y);
                jellyfish->health = 25;
                room.jellyfish_count++;
            }
            else{
                room.tiles[ROOM_WIDTH * y + x] = Tile_Empty;
            }
            curr += 3;
        }
    }

    return room;
}

Room transition(Player* player, i32 old_room_id, i32 new_room_id){
    Room new_room = load_room(new_room_id);

    for(i32 i = 0 ; i < new_room.transition_tile_count; ++i){
        if(new_room.transition_tiles[i].new_room_id == old_room_id){
            TransitionTile* exit_tile = &new_room.transition_tiles[i];
            Vector2 exit_direction = Vector2(0,0);
            if(exit_tile->pos_x == 0){
                exit_direction = Vector2(3,0);
            }else if(exit_tile->pos_y == 0){
                exit_direction = Vector2(0,3);
            }else if(exit_tile->pos_x == ROOM_WIDTH -1){
                exit_direction = Vector2(-3,0);
            }else if(exit_tile->pos_y == ROOM_HEIGHT -1){
                exit_direction = Vector2(0,-3);
            }

            player->position = Vector2(exit_tile->pos_x + exit_direction.x, exit_tile->pos_y + exit_direction.y);

        }
    }

    return new_room;
}
