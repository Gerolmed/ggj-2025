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

    for(u32 y = 0 ; y < ROOM_HEIGHT+4; ++y){
        for(u32 x = 0 ; x < ROOM_WIDTH+4; ++x){
            room.tiles[(ROOM_WIDTH+4) * y + x] = Tile_Wall;
        }
    }



    u8* curr = tmp;
    for (u32 y = 2; y < ROOM_HEIGHT+2; ++y) {
        for (u32 x = 2; x < ROOM_WIDTH+2; ++x) {
            //if(curr[0] == 0 && curr[1] == 0 && curr[2] == 0){
            //    room.tiles[(ROOM_WIDTH+4) * y + x] = Tile_Wall;
            //}
            //else{
                room.tiles[(ROOM_WIDTH+4) * y + x] = Tile_Empty;
            //}
            if(curr[0] == 255 && curr[1] == 255 && curr[2] < 64){
                Direction direction;

                if (x == 2)
                {
                    direction = Direction_Left;
                    room.tiles[(ROOM_WIDTH+4) * 5 + 0] = Tile_Empty;
                    room.tiles[(ROOM_WIDTH+4) * 5 + 1] = Tile_Empty;
                }
                if (x == ROOM_WIDTH + 1)
                {
                    direction = Direction_Right;
                    room.tiles[(ROOM_WIDTH+4) * 5 + ROOM_WIDTH+2] = Tile_Empty;
                    room.tiles[(ROOM_WIDTH+4) * 5 + ROOM_WIDTH+3] = Tile_Empty;
                }

                if (y == 2)
                {
                    direction = Direction_Up;
                    room.tiles[(ROOM_WIDTH+4) * 0 + 9] = Tile_Empty;
                    room.tiles[(ROOM_WIDTH+4) * 1 + 9] = Tile_Empty;
                    room.tiles[(ROOM_WIDTH+4) * 0 + 10] = Tile_Empty;
                    room.tiles[(ROOM_WIDTH+4) * 1 + 10] = Tile_Empty;
                }
                if (y == ROOM_HEIGHT + 1)
                {
                    direction = Direction_Down;
                    room.tiles[(ROOM_WIDTH+4) * (ROOM_HEIGHT+2) + 9] = Tile_Empty;
                    room.tiles[(ROOM_WIDTH+4) * (ROOM_HEIGHT+3) + 9] = Tile_Empty;
                    room.tiles[(ROOM_WIDTH+4) * (ROOM_HEIGHT+2) + 10] = Tile_Empty;
                    room.tiles[(ROOM_WIDTH+4) * (ROOM_HEIGHT+3) + 10] = Tile_Empty;
                }

                room.entrances[direction].enabled = true;
                room.entrances[direction].target_room = curr[2];
            }
            else if(curr[0] == 255 && curr[1] == 0 & curr[2] == 0){
                Pufferfish* pufferfish = &room.pufferfishs[room.pufferfish_count];
                pufferfish->position = Vector2(x, y);
                pufferfish->health.health = 16;
                pufferfish->health.damage_indicator = 0;
                room.pufferfish_count++;
            }
            else if(curr[0] == 100 && curr[1] == 100 && curr[2] == 100){
                Sharkfish* sharkfish = &room.sharkfishs[room.sharkfish_count];
                sharkfish->position = Vector2(x,y);
                sharkfish->health.health = 3;
                sharkfish->health.damage_indicator = 0;
                sharkfish->behavior_frame = 360 * room.sharkfish_count;
                sharkfish->upgraded = false;
                room.sharkfish_count++;
            }
            else if(curr[0] == 100 && curr[1] == 100 && curr[2] == 101){
                Sharkfish* sharkfish = &room.sharkfishs[room.sharkfish_count];
                sharkfish->position = Vector2(x,y);
                sharkfish->health.health = 5;
                sharkfish->health.damage_indicator = 0;
                sharkfish->behavior_frame = 300;
                sharkfish->upgraded = true;
                room.sharkfish_count++;
            }
            else if(curr[0] == 0 && curr[1] == 255 && curr[2] == 255){
                Jellyfish* jellyfish = &room.jellyfishs[room.jellyfish_count];
                jellyfish->position = Vector2(x,y);
                jellyfish->health.health = 25;
                jellyfish->health.damage_indicator = 0;
                jellyfish->behavior_frame = 60*room.jellyfish_count;
                jellyfish->animation_frame = 10*room.jellyfish_count;
                room.jellyfish_count++;
            }
            curr += 3;
        }
    }

//    for(i32 j = 0 ; j < ROOM_HEIGHT+4; j++){
//        for(i32 i= 0 ; i < ROOM_WIDTH+4; i++){
//            if(room.tiles[(ROOM_WIDTH+4) * j + i] == Tile_Wall){
//                printf("1");
//            }else if(room.tiles[(ROOM_WIDTH+4) * j + i] == Tile_Empty){
//                printf("0");
//            }else{
//                printf("2");
//            }
//        }
//        printf("\n");
//    }

    return room;
}
