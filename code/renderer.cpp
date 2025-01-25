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

enum ModelType
{
    Model_Toad,
    Model_Fish,
};

void RenderEntity(ModelType model, Vector2 pos, f32 rot)
{
    assert(state.render_entities.count < lengthof(state.render_entities.entities));
    EntityDraw *draw = state.render_entities.entities + state.render_entities.count++;
    *draw  = {};

    AllocateRenderSlot(draw);

    draw->x = pos.x;
    draw->y = pos.y;
}

void RenderAnimatedEntity(ModelType model, Vector2 pos, f32 rot, ModelAnimation *animation, u32 frame)
{
    assert(state.render_entities.count < lengthof(state.render_entities.entities));
    EntityDraw *draw = state.render_entities.entities + state.render_entities.count++;
    *draw = {};

    AllocateRenderSlot(draw);

    draw->x = pos.x;
    draw->y = pos.y;
}

