Room load_room(i32 room_id){
    Room room = {};

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
            else{
                room.tiles[ROOM_WIDTH * y + x] = Tile_Empty;
            }
            curr += 3;
        }
    }

    return room;
}
