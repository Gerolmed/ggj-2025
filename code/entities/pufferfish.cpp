

f32 fish_get_radius(Pufferfish* fish){
    return ((f32)7/(7+fish->health.health));
}

void fish_death(Pufferfish* fish, GameState* state){
    fish->health.dead = true;

    Room* room = state->rooms + state->current_room;

    Vector2 directions[4];
    directions[0] = Vector2(1,0);
    directions[1] = Vector2(0,1);
    directions[2] = Vector2(-1,0);
    directions[3] = Vector2(0,-1);

    for(i32 i = 0 ; i < 4; ++i)
    {
        //directions[i] = Vector2Rotate(directions[i], fish->rotation);

        ProjectileSpike spike = {};
        spike.position = fish->position;
        spike.direction = directions[i];

        StopMusicStream(step);
        PlayMusicStream(step);

        arrput(room->spikes, spike);
    }

}

void fish_pursue_player(Pufferfish* fish, GameState* state){
    Player* player = &state->player;
    Vector2 direction = Vector2Normalize(Vector2Subtract(player->position, fish->position));

    fish->position = Vector2Add(fish->position, Vector2Scale(direction, GetFrameTime()));
    fish->rotation = -Vector2Angle(direction, {1,0});
}

void fish_check_collision(Pufferfish* fish, GameState* state){
    SphericalCollider fish_collider = SphericalCollider(fish->position, fish_get_radius(fish));

    Room *level = state->rooms + state->current_room;

    //Bubble Projectile Collision
    ProjectileBubble *bubble_array = level->projectiles;
    for(i32 i = 0 ; i < arrlen(bubble_array); i++){
        ProjectileBubble bubble = bubble_array[i];
        SphericalCollider bubble_collider = SphericalCollider(bubble.position, bubble.radius);
        if(intersects(&fish_collider, &bubble_collider)){
            damage(&fish->health, bubble.damage);
            arrdel(bubble_array,i);
            i--;
        }
    }

    //Spike Projectile Collision
    ProjectileSpike *spikes_array = level->spikes;
    for(i32 i = 0 ; i < arrlen(spikes_array); i++){
        ProjectileSpike spike = spikes_array[i];
        SphericalCollider spike_collider = SphericalCollider(spike.position, SPIKE_RADIUS);
        if(intersects(&fish_collider, &spike_collider)){
            kill(&fish->health);
            arrdel(spikes_array,i);
            i--;
        }
    }
}

Pufferfish* get_living_pufferfish(GameState* state){
    Room* room = state->rooms + state->current_room;
    for(i32 i = 0; i < room->pufferfish_count; i++)
    {
        if(!room->pufferfishs[i].health.dead){
            return room->pufferfishs + i;
        }
    }

    return NULL;
}

void spawn_pufferfish(Vector2 position, GameState* state){
    Pufferfish* new_fish;
    
    Room* room = state->rooms + state->current_room;

    bool dead_fish_found = false;
    for(i32 i = 0; i < room->pufferfish_count; i++)
    {
        if(room->pufferfishs[i].health.dead){
            new_fish = room->pufferfishs + i;
            dead_fish_found = true;
        }
    }
    if(!dead_fish_found)
    {
        new_fish = room->pufferfishs + room->pufferfish_count;
        room->pufferfish_count++;
    }

    new_fish->position = position;
    new_fish->health.health = 16;
    new_fish->health.damage_indicator = 0;
    new_fish->health.dead = false;
}


void fish_update(Pufferfish* fish, GameState* state){
    if(fish->health.dead){
        return;
    }
    Vector2 old_pos = fish->position;

    fish_pursue_player(fish, state);
    fish_check_collision(fish, state);
    if(fish->health.dead){
        fish_death(fish, state);
    }

    update_health(&fish->health);

    collide_with_room(state->rooms + state->current_room, fish->position, old_pos, &fish->position);

    RenderEntity(Model_Fish, Vector2(fish->position.x, fish->position.y), 180 + fish->rotation * 180/PI, 2*fish_get_radius(fish), color_from_damage(&fish->health));
}