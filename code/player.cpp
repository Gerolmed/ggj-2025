void configure_player(Player* player)
{
    *player = {};
    player->speed = 5;
    player->health = 10;
    for (int i = 0; i < lengthof(player->bubbles); ++i)
    {
        player->bubbles[i] = Bubble{
            .radius = 1,
            .min_scale = 1.0f,
            .max_scale = 1.5f,
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
    float scale = bubble->max_scale * player->charge_value + bubble->min_scale * (1 - player->charge_value);
    direction = Vector2Scale(direction, bubble->radius * scale + 0.2f);
    return Vector2Add(player->position, direction);
}

f32 bubble_size(Player* player){
    return sqrt(player->charge_value * 4);
}

void update_charge_ball(Player* player)
{
    if (!player->charging) return;
    Bubble* bubble = player->bubbles + player->current_bubble;

    Vector2 position = get_current_bubble_position(player);

    // TODO: render ball?
    RenderEntity(Model_Bubble, position, 0, bubble_size(player));
}


void check_collisions(Player* player, GameState* state){
    SphericalCollider player_collider = SphericalCollider(player->position,1);

    //Pufferfish Collisions
    for(i32 i = 0 ; i < state->room.pufferfish_count; i++){
        Pufferfish* fish = &state->room.pufferfishs[i];
        if(fish->dead) continue;
        SphericalCollider fish_collider = {fish->position, fish_get_radius(fish)};

        if(intersects(&player_collider, &fish_collider))
        {
            player->health -= 1;
            printf("Damaged player");
            
        }
    }

    //Spike Collisions
    for(i32 i = 0 ; i < arrlen(state->room.spikes); i++){
        ProjectileSpike spike = state->room.spikes[i];

        SphericalCollider spike_collider = {spike.position, SPIKE_RADIUS};

        if(intersects(&player_collider, &spike_collider))
        {
            printf("Damaged player");
            player->health -= 1;
            arrdel(state->room.spikes,i);
            i--;
        }
    }
}


void execute_player_loop(Player* player, GameState* state)
{
    check_collisions(player,state);

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
            try_change_player_anim(player, PlayerAnim_Charge);
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
        projectile.damage = player->charge_value*10;
        projectile.velocity = direction * 10;
        arrput(state->room.projectiles, projectile);

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

    update_charge_ball(player);
    update_player_animation(player);

    RenderAnimatedEntity(Model_Toad, player->position, player->rotation, 1, player_model_animations + player->animation, player->frame);
    // RenderEntity(Model_Toad, player->position, 0);
}
