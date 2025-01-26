void AllocateRenderSlot(EntityDraw *draw)
{
    for (u32 x = 0; x < RENDER_ATLAS_SIZE; ++x)
    {
        for (u32 y = 0; y < RENDER_ATLAS_SIZE; ++y)
        {
            u32 index = x + y * RENDER_ATLAS_SIZE;

            if (!state.render_entities.tiles[index])
            {
                state.render_entities.tiles[index] = 1;

                draw->atlas_x = x;
                draw->atlas_y = y;
                return;
            }
        }
    }

    assert(0);
}

// 

void RenderEntity(ModelType model, Vector2 pos, f32 rot, f32 scale, Color color, ShadowSize shadow_size = ShadowSize_Medium)
{
    assert(state.render_entities.count < lengthof(state.render_entities.entities));
    EntityDraw *draw = state.render_entities.entities + state.render_entities.count++;
    *draw  = {};

    AllocateRenderSlot(draw);

    draw->x = pos.x;
    draw->y = pos.y;
    draw->model = model;
    draw->shadow_size = shadow_size;
    draw->rot = rot;
    draw->scale = scale;
    draw->color = color;
}

void RenderAnimatedEntity(ModelType model, Vector2 pos, f32 rot, f32 scale, ModelAnimation *animation, u32 frame, Color color, ShadowSize shadow_size = ShadowSize_Medium)
{
    assert(state.render_entities.count < lengthof(state.render_entities.entities));
    EntityDraw *draw = state.render_entities.entities + state.render_entities.count++;
    *draw = {};

    AllocateRenderSlot(draw);

    draw->x = pos.x;
    draw->y = pos.y;
    draw->model = model;
    draw->shadow_size = shadow_size;
    draw->rot = rot;
    draw->scale = scale;
    draw->animation = animation;
    draw->frame = frame;
    draw->color = color;
}

