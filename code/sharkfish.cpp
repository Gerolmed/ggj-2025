

void shark_death(Sharkfish* fish, GameState* state){
    fish->dead = true;
}

void pursue_player(Sharkfish* fish, GameState* state){
    Player* player = &state->player;
    Vector2 direction = Vector2Normalize(Vector2Subtract(player->position, fish->position));

    fish->position = Vector2Add(fish->position, Vector2Scale(direction, GetFrameTime()));
    fish->rotation = -Vector2Angle(direction, {1,0});
}


void shark_check_collision(Sharkfish* fish, GameState* state){
    SphericalCollider fish_collider = SphericalCollider(fish->position, 1);

    Room* level = &state->room;

    //Bubble Projectile Collision
    ProjectileBubble *bubble_array = level->projectiles;
    for(i32 i = 0 ; i < arrlen(bubble_array); i++){
        ProjectileBubble* bubble = &bubble_array[i];
        SphericalCollider bubble_collider = SphericalCollider(bubble->position, bubble->radius);
        if(intersects(&fish_collider, &bubble_collider)){
            bubble->velocity = Vector2Scale(bubble->velocity,-1);
            bubble->position = Vector2Add(bubble->position, Vector2Scale(bubble->velocity,GetFrameTime()));
        }
    }

    //Spike Projectile Collision
    ProjectileSpike *spikes_array = level->spikes;
    for(i32 i = 0 ; i < arrlen(spikes_array); i++){
        ProjectileSpike spike = spikes_array[i];
        SphericalCollider spike_collider = SphericalCollider(spike.position, SPIKE_RADIUS);
        if(intersects(&fish_collider, &spike_collider)){
            fish->health -= 1;
            arrdel(spikes_array,i);
            i--;
        }
    }
}

void shark_update(Sharkfish* fish, GameState* state){
    if(fish->dead) return;
    pursue_player(fish,state);
    shark_check_collision(fish,state);
    if(fish->health <= 0){
        shark_death(fish, state);
    }
}