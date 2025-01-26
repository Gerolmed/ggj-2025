void configure_player(Player* player)
{
    *player = {};
    player->speed = 5;
    player->position = {5, 5};

    player->health.max_health = 10;
    player->health.health = player->health.max_health;
    player->health.temp_health = 5;

    for (int i = 0; i < lengthof(player->bubbles); ++i)
    {
        player->bubbles[i] = Bubble{
            .radius = 0.5,
            .min_scale = 1.0f,
            .max_scale = 2.0f,
        };
    }
}


void try_change_player_anim(Player* player, PlayerAnim anim)
{
    if (player->animation == PlayerAnim_Charge || player->animation == PlayerAnim_PostShoot) return;
    player->animation = anim;
}

void update_player_animation(Player* player)
{
    player->frame++;
    const ModelAnimation animation = player_model_animations[player->animation];

    if (player->frame < animation.frameCount) return;

    if (player->animation == PlayerAnim_PostShoot)
    {
        player->frame = 0;
        player->animation = PlayerAnim_Idle;
    }
    else if (player->animation == PlayerAnim_Charge)
    {
        player->frame = animation.frameCount - 1;
    }
    else
    {
        player->frame = 0;
    }
}

Vector2 get_current_bubble_position(Player* player)
{
    Bubble* bubble = player->bubbles + player->current_bubble;

    Vector2 direction = Vector2Rotate({1, 0}, player->rotation);
    float scale = bubble->max_scale * sqrt(player->charge_value) + bubble->min_scale * (1 - sqrt(player->charge_value));
    direction = Vector2Scale(direction, bubble->radius * scale);
    return Vector2Add(player->position, direction);
}

f32 bubble_size(Player* player)
{
    return sqrt(player->charge_value * 4);
}

void update_charge_ball(Player* player)
{
    if (!player->charging) return;
    Bubble* bubble = player->bubbles + player->current_bubble;

    Vector2 position = get_current_bubble_position(player);

    RenderEntity(Model_Bubble, position, 0, bubble_size(player), WHITE);
}

inline void RoomTransition(Player* player, i32 new_room_id, Direction direction) 
{
    if (direction == Direction_Right)
    {
        player->position.x = 1;
    }

    if (direction == Direction_Left)
    {
        player->position.x = ROOM_WIDTH + 3;
    }

    if (direction == Direction_Up)
    {
        player->position.y = ROOM_HEIGHT + 3;
    }

    if (direction == Direction_Down)
    {
        player->position.y = 1;
    }

    state.current_room = new_room_id;
}

void check_collisions(Player* player, GameState* state)
{
    SphericalCollider player_collider = SphericalCollider(player->position, 0.5);

    Room* room = state->rooms + state->current_room;

    //Pufferfish Collisions
    for (i32 i = 0; i < room->pufferfish_count; i++)
    {
        Pufferfish* fish = room->pufferfishs + i;
        if (fish->health.dead) continue;
        SphericalCollider fish_collider = {fish->position, fish_get_radius(fish)};

        if (intersects(&player_collider, &fish_collider))
        {
            damage(&player->health, 1);
            player->knockback_velocity = Vector2Scale(Vector2Normalize(Vector2Subtract(player->position, fish->position)),2);
        }
    }

    //Sharkfish Collisions
    for (i32 i = 0; i < room->sharkfish_count; i++)
    {
        Sharkfish* fish = room->sharkfishs + i;
        if (fish->health.dead) continue;
        SphericalCollider fish_collider = {fish->position, 1};

        if (intersects(&player_collider, &fish_collider))
        {
            damage(&player->health, 3);
            player->knockback_velocity = Vector2Scale(Vector2Normalize(Vector2Subtract(player->position, fish->position)),10);
        }
    }

    //Spike Collisions
    for (i32 i = 0; i < arrlen(room->spikes); i++)
    {
        ProjectileSpike spike = room->spikes[i];

        SphericalCollider spike_collider = {spike.position, SPIKE_RADIUS};

        if (intersects(&player_collider, &spike_collider))
        {
            damage(&player->health, 1);
            arrdel(room->spikes, i);
            i--;
        }
    }

    //Bubble Collisions
    for (i32 i = 0; i < arrlen(room->projectiles); i++)
    {
        ProjectileBubble bubble = room->projectiles[i];
        if (!bubble.can_collide_with_player) continue;

        SphericalCollider bubble_collider = {bubble.position, bubble.radius};

        if (intersects(&player_collider, &bubble_collider))
        {
            damage(&player->health, bubble.damage);
            arrdel(room->projectiles, i);
            i--;
        }
    }

    if (room->entrances[Direction_Left].enabled && player->position.x < 1)
    {
        RoomTransition(player, room->entrances[Direction_Left].target_room, Direction_Left);
    }
    if (room->entrances[Direction_Right].enabled && player->position.x > ROOM_WIDTH + 3)
    {
        RoomTransition(player, room->entrances[Direction_Right].target_room, Direction_Right);
    }
    if (room->entrances[Direction_Up].enabled && player->position.y < 1)
    {
        RoomTransition(player, room->entrances[Direction_Up].target_room, Direction_Up);
    }
    if (room->entrances[Direction_Down].enabled && player->position.y > ROOM_HEIGHT + 3)
    {
        RoomTransition(player, room->entrances[Direction_Down].target_room, Direction_Down);
    }
}


void execute_player_loop(Player* player, GameState* state)
{
    Vector2 old_pos = player->position;


    check_collisions(player, state);

    Room* room = state->rooms + state->current_room;

    //Process knockback velocity
    if (player->knockback_velocity.x != 0 || player->knockback_velocity.y != 0)
    {
        player->position = Vector2Add(player->position, Vector2Scale(player->knockback_velocity, GetFrameTime()));
        Vector2 friction = Vector2Scale(Vector2Normalize(player->knockback_velocity), 10 * GetFrameTime());
        if (abs_squared(friction) > abs_squared(player->knockback_velocity))
        {
            player->knockback_velocity = {0, 0};
        }
        else
        {
            player->knockback_velocity = Vector2Subtract(player->knockback_velocity, friction);
        }
    }


    player->last_shot_age += GetFrameTime();

    const Vector2 mouse_screen_pos = GetMousePosition();
    const Vector2 player_screen_pos = GetWorldToScreen2D(player->position * TILE_SIZE_LOW, main_camera);
    Vector2 mouse_direction = Vector2Subtract(mouse_screen_pos, player_screen_pos);
    mouse_direction.y *= -1;
    player->rotation = Vector2Angle(mouse_direction, {1, 0});


    if (player->last_shot_age > player->fire_delay && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if (!player->charging)
        {
            player->frame = 0;
            player->animation = PlayerAnim_Charge;
        }
        player->charging = true;
        player->charge_value += GetFrameTime();
        player->charge_value = fmin(player->charge_value, 1);
    }

    if (player->charging && !IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        Vector2 position = get_current_bubble_position(player);
        Vector2 direction = Vector2Rotate({1, 0}, player->rotation);
        TraceLog(LOG_INFO, "%f", player->charge_value);
        //Shooting projectile
        ProjectileBubble projectile;
        projectile.position = position;
        projectile.radius = bubble_size(player);
        projectile.damage = player->charge_value * 10;
        projectile.can_collide_with_player = false;
        projectile.velocity = direction * 10;
        PlayMusicStream(bubble_sound[0]);
        arrput(room->projectiles, projectile);

        player->current_bubble = ++player->current_bubble % lengthof(player->bubbles);
        player->last_shot_age = 0;
        player->charge_value = 0;
        player->charging = false;

        player->frame = 0;
        player->animation = PlayerAnim_PostShoot;
    }

    try_change_player_anim(player, PlayerAnim_Idle);
    if (IsKeyDown(KEY_W))
    {
        player->position.y -= player->speed * GetFrameTime();
        try_change_player_anim(player, PlayerAnim_Walk);
    }
    else if (IsKeyDown(KEY_S))
    {
        player->position.y += player->speed * GetFrameTime();
        try_change_player_anim(player, PlayerAnim_Walk);
    }

    if (IsKeyDown(KEY_D))
    {
        player->position.x += player->speed * GetFrameTime();
        try_change_player_anim(player, PlayerAnim_Walk);
    }
    else if (IsKeyDown(KEY_A))
    {
        player->position.x -= player->speed * GetFrameTime();
        try_change_player_anim(player, PlayerAnim_Walk);
    }

    // Correct player position based on wall collisions
    collide_with_room(state->rooms + state->current_room, player->position, old_pos, &player->position);

    player->last_delta_pos = player->position - old_pos;

    update_charge_ball(player);
    update_player_animation(player);
    update_health(&player->health);

    RenderAnimatedEntity(Model_Toad, player->position, 180 + player->rotation * 180 / PI, 1,
                         player_model_animations + player->animation, player->frame,
                         color_from_damage(&player->health));
    // RenderEntity(Model_Toad, player->position, 0);
}

void draw_player_hud(const Player* player)
{
    Vector2 offset = {20, 20};
    float scale = 6;
    float sprite_size = texture_ui_heart_full.width * scale;
    float padding = 2;

    for (int i = 0; i < player->health.health / 2; i++)
    {
        DrawTextureEx(texture_ui_heart_full, offset, 0, scale, WHITE);
        offset += {sprite_size + padding, 0};
    }
    for (int i = 0; i < player->health.health % 2; i++)
    {
        DrawTextureEx(texture_ui_heart_half, offset, 0, scale, WHITE);
        offset += {sprite_size + padding, 0};
    }
    for (int i = 0; i < (player->health.max_health - player->health.health) / 2; i++)
    {
        DrawTextureEx(texture_ui_heart_empty, offset, 0, scale, WHITE);
        offset += {sprite_size + padding, 0};
    }

    for (int i = 0; i < player->health.temp_health / 2; i++)
    {
        DrawTextureEx(texture_ui_heart_temporary_full, offset, 0, scale, WHITE);
        offset += {sprite_size + padding, 0};
    }
    for (int i = 0; i < player->health.temp_health % 2; i++)
    {
        DrawTextureEx(texture_ui_heart_temporary_half, offset, 0, scale, WHITE);
        offset += {sprite_size + padding, 0};
    }
}
