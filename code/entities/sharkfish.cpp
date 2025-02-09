
i32 shark_animation_frame = 0;

void set_shark_animation(Sharkfish* fish, i32 animation){
    if(animation != fish->animation){
        shark_animation_frame = 0;
    }
    fish->animation = animation;
}

void shark_death(Sharkfish* fish, GameState* state){
    fish->health.dead = true;

    Vector2 item_offset[3];
    item_offset[0] = Vector2(0,0.2);
    item_offset[1] = Vector2(-0.3,-0.2);
    item_offset[2] = Vector2(0.3,-0.2);

    for(i32 i = 0 ; i < 3 ; i++){
        
        Room* room = state->rooms + state->current_room;
        configure_collectable(arraddnptr(room->collectables, 1), ItemType_Heart_Temp_Full, Vector2Add(fish->position,item_offset[i]));
    }
}

void pursue_player(Sharkfish* fish, GameState* state){
    fish->behavior_frame = (fish->behavior_frame + 1) % 600;
    Player* player = &state->player;
    Vector2 direction = Vector2Normalize(Vector2Subtract(player->position, fish->position));

    if(fish->behavior_frame % 300 < 180){
        set_shark_animation(fish,SharkAnim_Move);
        fish->rotation = -Vector2Angle(direction, {1,0});
        if(fish->health.health == 1) {
            fish->behavior_frame += 180;
        }

        if(fish->upgraded && fish->behavior_frame > 30){
            Pufferfish* pufferfish = get_living_pufferfish(state);
            if(pufferfish){
                Vector2 midpoint = Vector2Scale(Vector2Add(player->position, pufferfish->position) ,0.5f);
                direction = Vector2Scale(Vector2Normalize(Vector2Subtract(midpoint, fish->position)),2);
            }
        }
        fish->position = Vector2Add(fish->position, Vector2Scale(direction, GetFrameTime()));

    }else if (fish->behavior_frame % 300 < 240){
        fish->rotation = -Vector2Angle(direction, {1,0});
        fish->dash_direction = direction;
        set_shark_animation(fish,SharkAnim_Move);
    }else{
        set_shark_animation(fish,SharkAnim_Move);
        fish->position = Vector2Add(fish->position, Vector2Scale(fish->dash_direction, 7*GetFrameTime()));
    }

    if(fish->behavior_frame == 240){
        spawn_pufferfish(fish->position, state);
    }
    if(fish->upgraded && fish->behavior_frame == 540){
        spawn_jellyfish(fish->position, state);
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
                bubble->damage = 1;
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
            fish->behavior_frame = 180;
            if(fish->upgraded) fish->behavior_frame = 480;
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
    Vector2 old_pos = fish->position;

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
    shark_animation_frame++;

    ModelAnimation* animation = &shark_model_animations[fish->animation];


    update_health(&fish->health);
    collide_with_room(state->rooms + state->current_room, fish->position, old_pos, &fish->position);
    RenderAnimatedEntity(Model_Shark, Vector2(fish->position.x, fish->position.y), 180 + fish->rotation * 180/PI, 1.2, animation, shark_animation_frame % animation->frameCount , color_from_damage(&fish->health));
}