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

// todo: move to tracer.h ppm.h
void LoadImage(char *path, PpmBuffer* result){
  char fullPath[256];
  sprintf(fullPath, "./%s.ppm", path);
  //  sprintf(fullPath, "./tests/%s.ppm", path);
  
  // Open the file
  FILE* file = fopen(fullPath, "rb");
  if (!file) {
    printf("Error: Could not open file %s\n", fullPath);
    return;
  }
  
  // Read PPM header
  char format[3];
  int maxColor;
  
  // Read magic number (P3 or P6)
  fscanf(file, "%2s", format);
  if (format[0] != 'P' || (format[1] != '3' && format[1] != '6')) {
    printf("Error: Not a valid PPM file\n");
    fclose(file);
    return;
  }
  
  // Skip comments (lines starting with '#')
  char c = fgetc(file);
  while (c == '#') {
    while (c != '\n') c = fgetc(file);
    c = fgetc(file);
  }
  ungetc(c, file);
  
  // Read width and height
  fscanf(file, "%d %d", &result->pxWidth, &result->pxHeight);
  
  // Read max color value
  fscanf(file, "%d", &maxColor);
  
  // Allocate buffer for the image data
  result->buffer = (unsigned char*)malloc(result->pxWidth * result->pxHeight * 3);
  if (!result->buffer) {
    printf("Error: Memory allocation failed\n");
    fclose(file);
    return;
  }
  
  // Read the pixel data
  if (format[1] == '6') {
    // P6 - Binary format
    fread(result->buffer, 1, result->pxWidth * result->pxHeight * 3, file);
  } else {
    // P3 - ASCII format
    int r, g, b;
    for (int i = 0; i < result->pxWidth * result->pxHeight; i++) {
      fscanf(file, "%d %d %d", &r, &g, &b);
      result->buffer[i * 3 + 0] = (unsigned char)r;
      result->buffer[i * 3 + 1] = (unsigned char)g;
      result->buffer[i * 3 + 2] = (unsigned char)b;
    }
  }
  
  fclose(file);
}

void SaveImage(char* filename, PpmBuffer* ppm){
  ppmbuffer_save(filename, ppm);
}

void SaveSideBySide(PpmBuffer* stored, PpmBuffer* generated, char* suffix){
  
}

bool SameImage(PpmBuffer* stored, PpmBuffer* generated){
    if (!stored || !generated) {
        printf("Error: NULL pointer passed to SameImage\n");
        return false;
    }
    
    if (!stored->buffer || !generated->buffer) {
        printf("Error: One or both buffers are NULL\n");
        return false;
    }

    if (stored->pxWidth != generated->pxWidth || 
        stored->pxHeight != generated->pxHeight) {
        printf("Image dimensions differ: (%dx%d) vs (%dx%d)\n", 
               stored->pxWidth, stored->pxHeight, 
               generated->pxWidth, generated->pxHeight);
        return false;
    }
    
    int pixelCount = stored->pxWidth * stored->pxHeight * 3;
    for (int i = 0; i < pixelCount; i++) {

      //       printf("Stored %d generated %d", stored->buffer[i], generated->buffer[i]);
      
        if (stored->buffer[i] != generated->buffer[i]) {
            printf("Images differ at pixel %d (byte %d)\n", i / 3, i % 3);
            return false;
        }
    }
    
    return true;
}

// todo: review cleanp
MU_TEST(test_check) {
  
  mu_check(7 == 7);
  //  PRINT_NAME(test_check);  // Prints: "test_check"

  PpmBuffer loaded = {0};

  char filename[256];
  sprintf(filename, "%s/%s", SCREENSHOT_FOLDER, __func__);
  //  printf("LOADING: %s", filename);
  
  LoadImage(filename, &loaded);
  //  printf("Image loaded: %dx%d, buffer=%p\n", loaded.pxWidth, loaded.pxHeight, loaded.buffer);

  // generate image
  PpmBuffer generated = {0};
  generated.pxWidth = 640;
  generated.pxHeight = 480;

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
      1, INFINITY, recursion_depth ,
      spheres,sphereCount,
      lights, lightCount);
  

  // alteration
  /* for (int i = 0; i < pixelCount; i++) { */
  /*   generated.buffer[i*3 + 0] = 210; */
  /*   generated.buffer[i*3 + 1] = 210; */
  /*   generated.buffer[i*3 + 2] = 210; */
  /* } */

   
  char out_filename[256];
  sprintf(out_filename, "%s/%s.ppm", SCREENSHOT_FOLDER, __func__);
  SaveImage(out_filename, &generated);  

  return;
  // comparesment
  /* bool image_are_same = SameImage(&loaded, &generated); */
  /* mu_check(image_are_same); */
  
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
