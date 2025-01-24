
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

    PlayerAnim animation;
    i32 frame;
};




void test_player_loop()
{

}