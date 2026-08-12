#include <stdio.h>
#include "tracer.h"
#include "minunit.h"
#include <stdbool.h>
#include <string.h>

// windows.h stuff
#ifdef LoadImage
#undef LoadImage
#endif
// windows.h
#ifdef SaveImage
#undef SaveImage
#endif

#define SCREENSHOT_FOLDER "tests"
#define FAILED_SCREENSHOT_FOLDER "failed"
void pathToFailedScreenshotFromName(char* target, const char* testName){
  sprintf(target, "./%s/%s/%s.ppm", SCREENSHOT_FOLDER, FAILED_SCREENSHOT_FOLDER, testName);
}

void pathToScreenshotFromName(char* target, const char* testName){
  sprintf(target, "./%s/%s.ppm", SCREENSHOT_FOLDER, testName);
}

bool fileExists(char* path){
  FILE *file = fopen(path, "r");
  if (file != NULL) {
    return true;
    fclose(file);
  } else {
    return false;
  }
}
  
void CompareCombineSave(PpmBuffer* stored, PpmBuffer* generated, const char* path){
  PpmBuffer result;
  int width = stored->pxWidth;
  int height = stored->pxHeight;

  result.pxWidth = width * 3;
  result.pxHeight = height;
  // 3 bytes per pixel (RGB)
  result.buffer = (unsigned char*)malloc(result.pxWidth * result.pxHeight * 3); 
  
  if (result.buffer == NULL) {
    return;
  }

  
  ppmbuffer_compare_combine(stored, generated, &result);
  
  ppmbuffer_save(path, &result);
  free(result.buffer);
}

const Sphere DEFAULT_SPHERES[] = {
  // position
  {{0, -1, 3}, 1, {255, 0, 0}, 500, 0.2},
  {{2, 0, 4}, 1, {0, 0, 255}, 500, 0.3},
  {{-2, 0, 4}, 1, {0, 255, 0}, 10, 0.4},
  {{0, -5001, 0}, 5000, {255, 255, 0}, 1000, 0.5},
};

const Light DEFAULT_LIGHTS[] = {
  {LIGHT_AMBIENT, 0.2, {0, 0, 0}},
  {LIGHT_POINT, 0.6, {2, 1, 0}},
  {LIGHT_DIRECTIONAL, 0.2, {1, 4, 4}},
};

typedef struct{
  PpmBuffer generated;
  PpmBuffer loaded;
  Buffer render_buffer;
  Sphere spheres[4];
  int sphere_count;
  Light lights[3];
  int light_count;
  char test_name[256];
} RenderTestContext;


// Camera
const V3 DEFAULT_ORIGIN = {0};
const V3 DEFAULT_CAMERADIRECTION = {0,0,1};
const V3 DEFAULT_VIEWPORT = { 640, 480, 0};
const V3 DEFAULT_VIEWPORTSIZE = {1.0, 1.0, 1.0};
const float DEFAULT_PROJECTIONPLANE = 1.0;

// top, bot, left, right
// for default viewport
const Region DEFAULT_REGION = { 240, -240, -320, 320 };
const int DEFAULT_RECURSION_DEPTH = 3;
const Camera DEFAULT_CAMERA = {
  DEFAULT_ORIGIN,
  DEFAULT_CAMERADIRECTION,
  DEFAULT_VIEWPORTSIZE,
  DEFAULT_PROJECTIONPLANE
};


bool prepare_ppmBuffer(PpmBuffer* ppmBuffer){
  ppmBuffer->pxWidth = DEFAULT_VIEWPORT.x;
  ppmBuffer->pxHeight = DEFAULT_VIEWPORT.y;

  int pixelCount = ppmBuffer->pxWidth * ppmBuffer->pxHeight;
  int byteCount = pixelCount * 3;
  ppmBuffer->buffer = (unsigned char*)malloc(byteCount);
  if (!ppmBuffer->buffer) {
    printf("Memory allocation failed\n");    
    return 1;
  }
  
  return 0;
}

void prepare_renderBuffer(Buffer* renderBuffer, PpmBuffer* target){
  
  renderBuffer->width = target->pxWidth;
  renderBuffer->height = target->pxHeight;
  renderBuffer->size = renderBuffer->width * renderBuffer->height * 3;
  renderBuffer->start = target->buffer;  
}

RenderTestContext* create_context(const char* test_name){
  RenderTestContext* ctx = calloc(1, sizeof(RenderTestContext));

  prepare_ppmBuffer(&(ctx->generated));
  prepare_ppmBuffer(&(ctx->loaded));
  prepare_renderBuffer(&(ctx->render_buffer),&(ctx->generated));

  memcpy(ctx->spheres, DEFAULT_SPHERES, sizeof(DEFAULT_SPHERES));
  ctx->sphere_count = 4;
  
  memcpy(ctx->lights, DEFAULT_LIGHTS, sizeof(DEFAULT_LIGHTS));
  ctx->light_count = 3;

  // note: assume test_name < 256;
  strcpy(ctx->test_name, test_name);

  return ctx;
}

void render_test_scene(RenderTestContext* ctx){
  fillRegion(
	     DEFAULT_REGION,
	     DEFAULT_CAMERA,
	     ctx->render_buffer,
	     PIXEL_LAYOUT_RGB,
	     1, INFINITY,
	     DEFAULT_RECURSION_DEPTH,
	     ctx->spheres, ctx->sphere_count,
	     ctx->lights, ctx->light_count
	     );
}

// consider: exposing more verbosed fail reason
bool validate_test_result(RenderTestContext* ctx){
  char path[256];
  pathToScreenshotFromName(path, ctx->test_name);
  
  if(!fileExists(path)) {
    printf("Missing reference screnshot, wrigin generated:%s\n", path);
    ppmbuffer_save(path, &(ctx->generated));
    return false;
  }

  if(!ppmbuffer_load_into(path, &(ctx->loaded))){
    printf("Reference screnshot, failed to load\n");
    return false;
  }

  bool image_are_same = ppmbuffer_same(&(ctx->loaded), &(ctx->generated));
  if(!image_are_same){
    pathToFailedScreenshotFromName(path, ctx->test_name);
    CompareCombineSave(&(ctx->loaded),
		   &(ctx->generated),
    		   path);
  }

  return image_are_same;    
}

void destroy_render_test(RenderTestContext* ctx) {
    free(ctx->generated.buffer);
    free(ctx->loaded.buffer);
    free(ctx);
}

MU_TEST(default_scene) {
    RenderTestContext* ctx = create_context(__func__);
    
    render_test_scene(ctx);
    bool passed = validate_test_result(ctx);
    
    mu_check(passed);
    destroy_render_test(ctx);
}

#include "light.h"

MU_TEST_SUITE(test_suite) {
  MU_RUN_TEST(default_scene);
  
  MU_RUN_TEST(light_ambient);
  MU_RUN_TEST(light_point);
  MU_RUN_TEST(light_directional);  
}


int main(int argc, char *argv[]) {
  MU_RUN_SUITE(test_suite);
  MU_REPORT();
  return MU_EXIT_CODE;
}
