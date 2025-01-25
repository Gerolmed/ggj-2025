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

void RenderEntity(f32 x, f32 y)
{
    assert(state.render_entities.count < lengthof(state.render_entities.entities));
    EntityDraw *draw = state.render_entities.entities + state.render_entities.count++;

    AllocateRenderSlot(draw);

    draw->x = x;
    draw->y = y;
}

