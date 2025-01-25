f32 jelly_get_radius(Jellyfish* fish){
    return ((f32)10/(10+fish->health));
}

void jelly_death(Jellyfish* fish, GameState* state){
    fish->dead = true;
}

void orbit_around_player(Jellyfish* fish, GameState* state){
    fish->behavior_frame = (fish->behavior_frame + 1) % 180;
    Player* player = &state->player;

    Vector2 to_player = Vector2Subtract(player->position, fish->position);
    Vector2 direction = Vector2Normalize(to_player);
    fish->rotation = -Vector2Angle(direction, {1,0});

    if(fish->behavior_frame == 179){
        //Shoot bubble at player;
        ProjectileBubble projectile;
        projectile.position = Vector2Add(fish->position,Vector2Scale(direction, 0.1 + 2* jelly_get_radius(fish) ));
        projectile.radius = jelly_get_radius(fish);
        projectile.damage = 1;
        projectile.can_collide_with_player = true;
        projectile.velocity = direction * 10;
        arrput(state->room.projectiles, projectile);
    }

    if(abs_squared(to_player) < 5){
        direction = Vector2Scale(direction,-1);
    }else if(abs_squared(to_player) < 25){
        direction = Vector2(-direction.y,direction.x);
    }

    fish->position = Vector2Add(fish->position, Vector2Scale(direction, GetFrameTime()));
}


void jelly_check_collision(Jellyfish* fish, GameState* state){
    SphericalCollider fish_collider = SphericalCollider(fish->position, jelly_get_radius(fish));

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


void jellyfish_update(Jellyfish* fish, GameState* state){
    if(fish->dead) return;

    orbit_around_player(fish,state);
    jelly_check_collision(fish,state);
    if(fish->health <= 0){
        jelly_death(fish, state);
    }
}