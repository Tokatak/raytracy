#include <stdio.h>
#include "tracer.h"
#include "minunit.h"
#include <stdbool.h>

// windows.h stuff
#ifdef LoadImage
#undef LoadImage
#endif
// windows.h
#ifdef SaveImage
#undef SaveImage
#endif

#define SCREENSHOT_FOLDER "tests"

void appendExtention(char* target,const char* fileName){
  sprintf(target, "%s.ppm", fileName);
}

void appendFolder(char* target, char* fileName){
  sprintf(target, "./%s/%s",SCREENSHOT_FOLDER, fileName);
}

void screenshotFromTestname(char* target, const char* testName){
  char tmp[256];
  appendExtention(tmp, testName);
  appendFolder(target,tmp);
}

void screenshotFromTestnameFailed(char* target, const char* testName){
  // todo
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
  

void SaveSideBySide(PpmBuffer* stored, PpmBuffer* generated, const char* suffix){
  char path[256]; 

  // todo: push path handling to one place
  sprintf(path, "./tests/failed/%s.ppm",  (suffix != NULL) ? suffix : "");

  PpmBuffer diff;
  ppmbuffer_compare_combine(stored, generated, &diff);
  
  ppmbuffer_save(path, &diff);
  free(diff.buffer);
}

const V3 DEFAULT_VIEWPORT = { 640, 480, 0};
const V3 DEFAULT_ORIGIN = {0};
const V3 DEFAULT_CAMERADIRECTION = {0,0,1};
// top, bot, left, right
// for default viewport
const Region DEFAULT_REGION = { 240, -240, -320, 320 };
const V3 DEFAULT_VIEWPORTSIZE = {1.0, 1.0, 1.0};
const float DEFAULT_PROJECTIONPLANE = 1.0;
const int DEFAULT_RECURSION_DEPTH = 3;

const Sphere DEFAULT_SPHERES[] = {
  // position
  {{0, -1, 3}, 1, {255, 0, 0}, 500, 0.2},
  {{2, 0, 4}, 1, {0, 0, 255}, 500, 0.3},
  {{-2, 0, 4}, 1, {0, 255, 0}, 10, 0.4},
  {{0, -5001, 0}, 5000, {255, 255, 0}, 1000, 0.5},
};
static const int DEFAULT_SPHERE_COUNT = sizeof(DEFAULT_SPHERES) / sizeof(DEFAULT_SPHERES[0]);

const Light DEFAULT_LIGHTS[] = {
  {LIGHT_AMBIENT, 0.2, {0, 0, 0}},
  {LIGHT_POINT, 0.6, {2, 1, 0}},
  {LIGHT_DIRECTIONAL, 0.2, {1, 4, 4}},
};
static const int DEFAULT_LIGHT_COUNT = sizeof(DEFAULT_LIGHTS) / sizeof(DEFAULT_LIGHTS[0]);

// todo: rename 
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

// todo: review cleanp this mess
MU_TEST(test_check) {  
  PpmBuffer generated = {0};
  if(prepare_ppmBuffer(&generated)){    
    mu_fail("Test failed. Failed to prepare generated buffer.");
    return;
  }

  Buffer render_buffer = {0};
  prepare_renderBuffer(&render_buffer, &generated);
  
  PpmBuffer loaded = {0};
  if(prepare_ppmBuffer(&loaded)){
    mu_fail("Test failed. Failed to prepared loaded buffer.");
    return;
  }  
  
  Sphere spheres[DEFAULT_SPHERE_COUNT];
  memcpy(spheres, DEFAULT_SPHERES, sizeof(DEFAULT_SPHERES));
  int sphere_count = DEFAULT_SPHERE_COUNT;

  Light lights[DEFAULT_LIGHT_COUNT];
  memcpy(lights, DEFAULT_LIGHTS, sizeof(DEFAULT_LIGHTS));
  int light_count = DEFAULT_LIGHT_COUNT;
  
  fillRegion
    ( DEFAULT_ORIGIN,
      DEFAULT_CAMERADIRECTION,
      DEFAULT_REGION,
      DEFAULT_VIEWPORTSIZE,
      DEFAULT_PROJECTIONPLANE,
      render_buffer,
      1, INFINITY,
      DEFAULT_RECURSION_DEPTH,
      spheres, sphere_count,
      lights, light_count);


  // loading compare sample
  char screenshotPath[256];
  screenshotFromTestname(screenshotPath, __func__);

  if(!fileExists(screenshotPath)) {
    char tmp[512];
    sprintf(tmp,"Missing reference screnshot, wrigin generated:%s\n", screenshotPath);
    ppmbuffer_save(screenshotPath, &generated);
    mu_fail(tmp);
    return;
  }

  if(!ppmbuffer_load_into(screenshotPath, &loaded)){
    char tmp[512];
    sprintf(tmp,"Reference screnshot, failed to load\n");
    mu_fail(tmp);
    return;
  }

  bool image_are_same = ppmbuffer_same(&loaded, &generated);
  if(!image_are_same){
    SaveSideBySide(&loaded, &generated, __func__);
  }
  
  mu_check(image_are_same);

  free(generated.buffer);
  free(loaded.buffer);
}

MU_TEST_SUITE(test_suite) {
  MU_RUN_TEST(test_check);
}

int main(int argc, char *argv[]) {
  MU_RUN_SUITE(test_suite);
  MU_REPORT();
  return MU_EXIT_CODE;
}
