// Lights:
MU_TEST(light_ambient) {
    RenderTestContext* ctx = create_context(__func__);

    // only ambient 
    ctx->lights[0] = DEFAULT_LIGHTS[0];
    ctx->light_count = 1 ;    
    
    render_test_scene(ctx);
    bool passed = validate_test_result(ctx);
    
    mu_check(passed);
    destroy_render_test(ctx);
}

MU_TEST(light_point) {
    RenderTestContext* ctx = create_context(__func__);

    // only point
    ctx->lights[0] = DEFAULT_LIGHTS[1];
    ctx->light_count = 1 ;    
    
    render_test_scene(ctx);
    bool passed = validate_test_result(ctx);
    
    mu_check(passed);
    destroy_render_test(ctx);
}

MU_TEST(light_directional) {
    RenderTestContext* ctx = create_context(__func__);

    // only point
    ctx->lights[0] = DEFAULT_LIGHTS[2];
    ctx->light_count = 1 ;    
    
    render_test_scene(ctx);
    bool passed = validate_test_result(ctx);
    
    mu_check(passed);
    destroy_render_test(ctx);
}
