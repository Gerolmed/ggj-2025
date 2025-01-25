
struct Bubble
{

};

enum PlayerAnim
{
    PlayerAnim_Idle,
    PlayerAnim_Walk,
    PlayerAnim_Charge,
    PlayerAnim_PostShoot,
};

struct Player {
    Bubble bubbles[6];
    i32 current_bubble;

    Vector2 position;
    f32 rotation;

    bool charging;
    f32 charge_age;
    f32 last_shot_age;

    PlayerAnim animation;
    i32 frame;

    f32 speed;
    f32 fire_delay;
};

void try_change_player_anim(Player *player, PlayerAnim anim)
{
    if (player->animation == PlayerAnim_Charge || player->animation == PlayerAnim_PostShoot) return;
    player->animation = anim;
}

void update_player_animation(Player *player)
{
    player->frame++;

    // TODO: check for animation end

    if (player->animation == PlayerAnim_PostShoot)
    {
        player->frame = 0;
        player->animation = PlayerAnim_Idle;
        return;
    }

    // loop animation
}


void test_player_loop(Player *player)
{
    player->last_shot_age += GetFrameTime();
    main_camera.target

    Vector2 mouse_screen_pos = GetMousePosition();


    if (player->last_shot_age > player->fire_delay && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (!player->charging)
        {
            player->frame = 0;
            try_change_player_anim(player, PlayerAnim_Charge);
        }
        player->charging = true;
        player->charge_age += GetFrameTime();
    }

    if (player->charging && !IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        // TODO: shoot projectile

        player->current_bubble = ++player->current_bubble % sizeof(player->bubbles);
        player->last_shot_age = 0;
        player->charge_age = 0;

        player->frame = 0;
        player->animation = PlayerAnim_PostShoot;
    }


    try_change_player_anim(player, PlayerAnim_Idle);
    if (IsKeyPressed(KEY_W))
    {
       player->position.y += player->speed * GetFrameTime();
        try_change_player_anim(player, PlayerAnim_Walk);
    } else if (IsKeyPressed(KEY_S))
    {
        player->position.y -= player->speed * GetFrameTime();
        try_change_player_anim(player, PlayerAnim_Walk);
    }

    if (IsKeyPressed(KEY_D))
    {
        player->position.x += player->speed * GetFrameTime();
        try_change_player_anim(player, PlayerAnim_Walk);
    } else if (IsKeyPressed(KEY_A))
    {
        player->position.x -= player->speed * GetFrameTime();
        try_change_player_anim(player, PlayerAnim_Walk);
    }

    update_player_animation(player);
}