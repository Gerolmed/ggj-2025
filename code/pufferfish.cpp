

f32 fish_get_radius(Pufferfish* fish){
    return ((f32)10/(10+fish->health));
}

void fish_death(Pufferfish* fish){
    fish->dead = true;
    //TODO: Explode and spawn spike projectiles
}

void fish_pursue_player(){
    //TODO
}


void fish_check_collision(Pufferfish* fish, Room* level){
    SphericalCollider fish_collider = SphericalCollider(fish->position, fish_get_radius(fish));



    //Bubble Projectile Collision
    ProjectileBubble *bubble_array = level->projectiles;
    for(i32 i = 0 ; i < arrlen(bubble_array); i++){
        ProjectileBubble bubble = bubble_array[i];
        SphericalCollider bubble_collider = SphericalCollider(bubble.position, bubble.radius);
        if(intersects(&fish_collider, &bubble_collider)){
            fish->health -= bubble.damage;
            if(fish->health <= 0){
                fish_death(fish);
            }
            arrdel(bubble_array,i);
            i--;
        }
    }
}


void fish_update(Pufferfish* fish, Room* level){
    if(fish->dead){
        return;
    }
    fish_pursue_player();
    fish_check_collision(fish, level);
}