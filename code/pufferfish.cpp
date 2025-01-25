

f32 fish_get_radius(Pufferfish* fish){
    return ((f32)7/(7+fish->health));
}

void fish_death(Pufferfish* fish, GameState* state){
    fish->dead = true;

    Vector2 directions[4];
    directions[0] = Vector2(1,0);
    directions[1] = Vector2(0,1);
    directions[2] = Vector2(-1,0);
    directions[3] = Vector2(0,-1);

    for(i32 i = 0 ; i < 4; ++i)
    {
        directions[i] = Vector2Rotate(directions[i], fish->rotation);

        ProjectileSpike spike = {};
        spike.position = fish->position;
        spike.direction = directions[i];

        arrput(state->room.spikes, spike);
    }

    //TODO: Explode and spawn spike projectiles
}

void fish_pursue_player(Pufferfish* fish, GameState* state){
    Player* player = &state->player;
    Vector2 direction = Vector2Normalize(Vector2Subtract(player->position, fish->position));

    fish->position = Vector2Add(fish->position, Vector2Scale(direction, GetFrameTime()));
    fish->rotation = -Vector2Angle(direction, {1,0});
}

void fish_check_collision(Pufferfish* fish, GameState* state){
    SphericalCollider fish_collider = SphericalCollider(fish->position, fish_get_radius(fish));

    Room* level = &state->room;

    //Bubble Projectile Collision
    ProjectileBubble *bubble_array = level->projectiles;
    for(i32 i = 0 ; i < arrlen(bubble_array); i++){
        ProjectileBubble bubble = bubble_array[i];
        SphericalCollider bubble_collider = SphericalCollider(bubble.position, bubble.radius);
        if(intersects(&fish_collider, &bubble_collider)){
            fish->health -= bubble.damage;
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
            fish->health = 0;
            arrdel(spikes_array,i);
            i--;
        }
    }
}


void fish_update(Pufferfish* fish, GameState* state){
    if(fish->dead){
        return;
    }
    fish_pursue_player(fish, state);
    fish_check_collision(fish, state);
    if(fish->health <= 0){
        fish_death(fish, state);
    }
}