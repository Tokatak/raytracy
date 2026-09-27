//Depth test:
MU_TEST(depth) {
    RenderTestContext* ctx = create_context(__func__);

    // only ambient 
    ctx->lights[0] = DEFAULT_LIGHTS[0];
    ctx->light_count = 1 ;    
    
    render_test_scene_depth(ctx);
    bool passed = validate_test_result(ctx);
    
    mu_check(passed);
    destroy_render_test(ctx);
}
