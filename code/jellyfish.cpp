f32 jelly_get_radius(Jellyfish* fish){
    return ((f32)10/(10+fish->health.health));
}

void jelly_death(Jellyfish* fish, GameState* state){
    fish->health.dead = true;
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
        projectile.damage = 1;
        projectile.can_collide_with_player = true;
        projectile.velocity = direction * 10;
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
            fish->health.health -= bubble.damage;
            fish->health.damage_indicator = 1;
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
            fish->health.health -= 5;
            arrdel(spikes_array,i);
            i--;
        }
    }
}


void jellyfish_update(Jellyfish* fish, GameState* state){
    if(fish->health.dead) return;

    orbit_around_player(fish,state);
    jelly_check_collision(fish,state);
    if(fish->health.health <= 0){
        jelly_death(fish, state);
    }
}