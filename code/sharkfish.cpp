

void shark_death(Sharkfish* fish, GameState* state){
    fish->health.dead = true;
}

void pursue_player(Sharkfish* fish, GameState* state){
    fish->behavior_frame = (fish->behavior_frame + 1) % 600;
    Player* player = &state->player;
    Vector2 direction = Vector2Normalize(Vector2Subtract(player->position, fish->position));

    if(fish->behavior_frame % 300 < 180){
        if(fish->health.health == 1) fish->behavior_frame += 180;
        fish->position = Vector2Add(fish->position, Vector2Scale(direction, GetFrameTime()));
        fish->rotation = -Vector2Angle(direction, {1,0});
    }else if (fish->behavior_frame % 300 < 240){
        fish->rotation = -Vector2Angle(direction, {1,0});
        fish->dash_direction = direction;
    }else{
        fish->position = Vector2Add(fish->position, Vector2Scale(fish->dash_direction, 7*GetFrameTime()));
    }

    if(fish->behavior_frame == 240){
        spawn_pufferfish(fish->position, state);
    }
}


void shark_check_collision(Sharkfish* fish, GameState* state){
    SphericalCollider fish_collider = SphericalCollider(fish->position, 0.5);

    Room* level = state->rooms + state->current_room;

    //Bubble Projectile Collision
    ProjectileBubble *bubble_array = level->projectiles;
    for(i32 i = 0 ; i < arrlen(bubble_array); i++){
        ProjectileBubble* bubble = &bubble_array[i];
        SphericalCollider bubble_collider = SphericalCollider(bubble->position, bubble->radius);
        if(intersects(&fish_collider, &bubble_collider)){
            if(!bubble->can_collide_with_player){
                bubble->velocity = Vector2Scale(bubble->velocity,-1);
                bubble->can_collide_with_player = true;
            }
        }
    }

    //Spike Projectile Collision
    ProjectileSpike *spikes_array = level->spikes;
    bool hit = false;
    for(i32 i = 0 ; i < arrlen(spikes_array); i++){
        ProjectileSpike spike = spikes_array[i];
        SphericalCollider spike_collider = SphericalCollider(spike.position, SPIKE_RADIUS);
        if(intersects(&fish_collider, &spike_collider)){
            hit = true;
            fish->knockback_velocity = Vector2Scale(spike.direction,5);
            fish->behavior_frame = 480;
            PlayMusicStream(cry);
            arrdel(spikes_array,i);
            i--;
        }
    }
    if(hit)
    {
        damage(&fish->health, 1);
    }
}

void shark_update(Sharkfish* fish, GameState* state){
    if(fish->health.dead) return;

      //Process knockback velocity
    if(fish->knockback_velocity.x != 0 || fish->knockback_velocity.y != 0)
    {
        fish->position = Vector2Add(fish->position, Vector2Scale(fish->knockback_velocity, GetFrameTime()));
        Vector2 friction = Vector2Scale(Vector2Normalize(fish->knockback_velocity), 5* GetFrameTime());
        if(abs_squared(friction) > abs_squared(fish->knockback_velocity)){
            fish->knockback_velocity = {0,0};
        }else{
            fish->knockback_velocity = Vector2Subtract(fish->knockback_velocity, friction);
        }
    }

    pursue_player(fish,state);
    shark_check_collision(fish,state);
    if(fish->health.dead){
        shark_death(fish, state);
    }

    update_health(&fish->health);
    RenderEntity(Model_Shark, Vector2(fish->position.x, fish->position.y), 180 + fish->rotation * 180/PI, 1, color_from_damage(&fish->health));
}