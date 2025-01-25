

f32 fish_get_radius(Pufferfish* fish){
    return TILE_SIZE_LOW*((f32)10/(10+fish->health));
}


