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

void appendExtention(char* target,const char* filename){
  sprintf(target, "%s.ppm", filename);
}

void appendFolder(char* target, char* filename){
  sprintf(target, "./%s/%s",SCREENSHOT_FOLDER, filename);
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


// todo: review cleanp this mess
MU_TEST(test_check) {
  
  mu_check(7 == 7);
  
  // generate image
  PpmBuffer generated = {0};
  /* generated.pxWidth = 640; */
  /* generated.pxHeight = 480; */

  generated.pxWidth = 100;
  generated.pxHeight = 100;

  int pixelCount = generated.pxWidth * generated.pxHeight;
  int byteCount = pixelCount * 3;
 
  generated.buffer = (unsigned char*)malloc(byteCount);
  if (!generated.buffer) {
    printf("Memory allocation failed\n");
    //todo:  Handle error
  }

  // fiil region here
  V3 origin = {0};
  V3 cameraDirection = {0,0,1};
  
  int topEdge = generated.pxHeight / 2;
  int bottomEdge = -generated.pxHeight / 2;
  int leftEdge = -generated.pxWidth / 2;
  int righEdge = generated.pxWidth / 2;
  Region region;
  region.top = topEdge;
  region.bot = bottomEdge;
  region.left = leftEdge;
  region.right = righEdge;
  V3 viewportSize = {1.0, 1.0, 0.0};
  float projectionPlane = 1.0;
  Buffer buffer = {0};
  buffer.width = generated.pxWidth;
  buffer.height = generated.pxHeight;
  buffer.size = buffer.width * buffer.height * 3;
  buffer.start = generated.buffer;
  int recursion_depth = 3;

  Sphere spheres[] = {
    // position
    {{0, -1, 3}, 1, {255, 0, 0}, 500, 0.2},
    {{2, 0, 4}, 1, {0, 0, 255}, 500, 0.3},
    {{-2, 0, 4}, 1, {0, 255, 0}, 10, 0.4},
    {{0, -5001, 0}, 5000, {255, 255, 0}, 1000, 0.5},
  };
  int sphereCount = 4;

  // todo: fix no ligth exception
  Light lights[] = {
    {LIGHT_AMBIENT, 0.2, {0, 0, 0}},
    {LIGHT_POINT, 0.6, {2, 1, 0}},
    {LIGHT_DIRECTIONAL, 0.2, {1, 4, 4}},
  };
  int lightCount = 3;
  
  fillRegion
    ( origin, cameraDirection, region, viewportSize, projectionPlane,
      buffer,
      1, INFINITY, recursion_depth,
      spheres,sphereCount,
      lights, lightCount);

  // at this point image is generated

  // loading compare sample
  PpmBuffer loaded = {0};


  char filenameWithExtension[256];
  appendExtention(filenameWithExtension, __func__);
  char filenameWithFolder[256];
  appendFolder(filenameWithFolder,filenameWithExtension);
  //  sprintf(filename, "%s/%s", SCREENSHOT_FOLDER, __func__);

  bool exists  = fileExists(filenameWithFolder);

  if( !exists ) {
    bool reference_screenshot_missing = !exists;
    // writing rendered
    printf("Missing reference screnshot, wrigin generated:%s\n", filenameWithFolder);
    ppmbuffer_save(filenameWithFolder, &generated);  
    mu_check(reference_screenshot_missing);
    return;
  }

  bool image_loaded = ppmbuffer_load(filenameWithFolder, &loaded);
  if(!image_loaded){
    bool reference_screenshot_failed_to_load = !image_loaded;
    printf("Reference screnshot, failed to load\n");
    mu_check(reference_screenshot_failed_to_load);
  }

  // compare
  bool image_are_same = ppmbuffer_same(&loaded, &generated);
  if( !image_are_same){
    SaveSideBySide(&loaded, &generated, __func__);
  }
  mu_check(image_are_same);


  /* char out_filename[256]; */
  /* sprintf(out_filename, "%s/%s.ppm", SCREENSHOT_FOLDER, __func__); */
  /* SaveImage(out_filename, &loaded); */
}

MU_TEST_SUITE(test_suite) {
  MU_RUN_TEST(test_check);
}

int main(int argc, char *argv[]) {
  MU_RUN_SUITE(test_suite);
  MU_REPORT();
  return MU_EXIT_CODE;
}
