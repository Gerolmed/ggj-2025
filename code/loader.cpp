Room load_room(){
    Room result = {};

    i32 width;
    i32 height;

    char path[1024];
    sprintf(path, "asset/rooms/0.png");
    u8* tmp = stbi_load(path, (i32*) &width, (i32*) &height, NULL, STBI_rgb);
    assert(tmp);

    u8* curr = tmp;
    for (u32 y = 0; y < ROOM_HEIGHT; ++y) {
        for (u32 x = 0; x < ROOM_WIDTH; ++x) {
            if(curr[0]== 0 && curr[1] == 0 && curr[2] == 0){
                result.tiles[ROOM_WIDTH * y + x] = Tile_Wall;
            }else{
                result.tiles[ROOM_WIDTH * y + x] = Tile_Empty;
            }
            curr += 3;
        }
    }

    return result;
}
