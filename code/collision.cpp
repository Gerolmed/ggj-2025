
f32 abs_squared(Vector2 v){
    return v.x * v.x + v.y * v.y;
}

bool intersects(SphericalCollider* collider1, SphericalCollider* collider2){
    return abs_squared(Vector2Subtract(collider1->position, collider2->position)) < collider1->radius + collider2->radius;
}

