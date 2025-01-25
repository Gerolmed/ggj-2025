
f32 abs_squared(Vector2 v){
    return v.x * v.x + v.y * v.y;
}

Vector2 vector2_sub(Vector2 v1, Vector2 v2){
    return Vector2(v1.x - v2.x, v1.y - v2.y);
}

bool intersects(SphericalCollider* collider1, SphericalCollider* collider2){
    return abs_squared(vector2_sub(collider1->position, collider2->position)) < collider1->radius + collider2->radius;
}

