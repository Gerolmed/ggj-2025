f32 jelly_get_radius(Jellyfish* fish){
    return ((f32)10/(10+fish->health.health));
}

void jelly_death(Jellyfish* fish, GameState* state){
    fish->health.dead = true;

    Room* room = state->rooms + state->current_room;
    configure_collectable(arraddnptr(room->collectables, 1), ItemType_Heart_Full, fish->position);
}

void orbit_around_player(Jellyfish* fish, GameState* state){
    Room* room = state->rooms + state->current_room;

    fish->behavior_frame = (fish->behavior_frame + 1) % 180;
    Player* player = &state->player;

    Vector2 to_player = Vector2Subtract(player->position, fish->position);
    Vector2 direction = Vector2Normalize(to_player);
    f32 orthogonal_speed = 4;
    if(fish->behavior_frame < 90){
        orthogonal_speed = 1;
    }
    Vector2 orthogonal = {-orthogonal_speed* direction.y , orthogonal_speed* direction.x};
    fish->rotation = -Vector2Angle(direction, {1,0});

    if(fish->behavior_frame == 179){
        //Shoot bubble at player;
        ProjectileBubble projectile;
        projectile.position = fish->position;
        projectile.radius = 2*jelly_get_radius(fish);
        projectile.damage = 2;
        if(fish->health.health < 5) projectile.damage = 4;
        projectile.can_collide_with_player = true;
        projectile.velocity = direction * 10;
        StopMusicStream(jump[0]);
        PlayMusicStream(jump[0]);
        arrput(room->projectiles, projectile);
    }

    if(abs_squared(to_player) < 5){
        direction = Vector2Scale(direction,-1);
    }else if(abs_squared(to_player) < 25){
        direction = Vector2(0,0);
    }

    direction = Vector2Add(direction, orthogonal);

    fish->position = Vector2Add(fish->position, Vector2Scale(direction, GetFrameTime()));
}


void jelly_check_collision(Jellyfish* fish, GameState* state){
    SphericalCollider fish_collider = SphericalCollider(fish->position, jelly_get_radius(fish));

    Room* level = state->rooms + state->current_room;

    //Bubble Projectile Collision
    ProjectileBubble *bubble_array = level->projectiles;
    for(i32 i = 0 ; i < arrlen(bubble_array); i++){
        ProjectileBubble bubble = bubble_array[i];
        if(bubble.can_collide_with_player) continue;
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

void spawn_jellyfish(Vector2 position, GameState* state){
    Jellyfish* new_fish;
    
    Room* room = state->rooms + state->current_room;

    bool dead_fish_found = false;
    for(i32 i = 0; i < room->jellyfish_count; i++)
    {
        if(room->jellyfishs[i].health.dead){
            new_fish = room->jellyfishs + i;
            dead_fish_found = true;
        }
    }
    if(!dead_fish_found)
    {
        new_fish = room->jellyfishs + room->jellyfish_count;
        room->jellyfish_count++;
    }

    new_fish->position = position;
    new_fish->health.health = 16;
    new_fish->health.damage_indicator = 0;
    new_fish->health.dead = false;
}



void jellyfish_update(Jellyfish* fish, GameState* state){
    if(fish->health.dead) return;
    Vector2 old_pos = fish->position;

    orbit_around_player(fish,state);
    jelly_check_collision(fish,state);
     if(fish->health.dead){
        jelly_death(fish, state);
    }

    update_health(&fish->health);
    collide_with_room(state->rooms + state->current_room, fish->position, old_pos, &fish->position);
    RenderEntity(Model_Jelly, Vector2(fish->position.x, fish->position.y), 180 + fish->rotation * 180/PI, 2*jelly_get_radius(fish), color_from_damage(&fish->health));
}