void configure_player(Player* player)
{
    *player = {};
    player->speed = 5;
    player->bubbles[0] = Bubble{
        .radius = 1,
        .min_scale = 1.0f,
        .max_scale = 1.5f,
    };
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

void update_charge_ball(Player* player)
{
    if (!player->charging) return;
    Bubble* bubble = player->bubbles + player->current_bubble;

    Vector2 position = get_current_bubble_position(player);

    // TODO: render ball?
}

void execute_player_loop(Player* player, GameState* state)
{
    player->last_shot_age += GetFrameTime();

    const Vector2 mouse_screen_pos = GetMousePosition();
    const Vector2 player_screen_pos = GetWorldToScreen2D(player->position, main_camera);
    const Vector2 mouse_direction = Vector2Subtract(mouse_screen_pos, player_screen_pos);
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
        Vector2 direction = Vector2Normalize(mouse_direction);

        //Shooting projectile
        ProjectileBubble projectile;
        projectile.position = player->position;
        projectile.radius = player->charge_value * 10;
        projectile.damage = projectile.radius;
        projectile.velocity = direction * 3;
        arrput(state->room.projectiles, projectile);

        player->current_bubble = ++player->current_bubble % sizeof(player->bubbles);
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

    update_player_animation(player);

    RenderAnimatedEntity(Model_Toad, player->position, player->rotation, player_model_animations + player->animation, player->frame);
    // RenderEntity(Model_Toad, player->position, 0);
}
