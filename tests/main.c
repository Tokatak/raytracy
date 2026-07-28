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
  save_ppmbuffer(filename, ppm);
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

      printf("Stored %d generated %d", stored->buffer[i], generated->buffer[i]);
      
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
  printf("LOADING: %s", filename);
  
  LoadImage(filename, &loaded);
  printf("Image loaded: %dx%d, buffer=%p\n", loaded.pxWidth, loaded.pxHeight, loaded.buffer);

  // generate image
  PpmBuffer generated = {0};
  generated.pxWidth = 9;
  generated.pxHeight = 9;

  int pixelCount = generated.pxWidth * generated.pxHeight;
  int byteCount = pixelCount * 3;
 
  generated.buffer = (unsigned char*)malloc(byteCount);
  if (!generated.buffer) {
    printf("Memory allocation failed\n");
    // Handle error
  }
  
  for (int i = 0; i < pixelCount; i++) {
    generated.buffer[i*3 + 0] = 210;
    generated.buffer[i*3 + 1] = 210;
    generated.buffer[i*3 + 2] = 210;
  }

  printf("Image loaded: %dx%d, buffer=%p\n", generated.pxWidth, generated.pxHeight,
	 generated.buffer);

  bool result = SameImage(&loaded, &generated);
  printf("Same image %s", result ? "true":"false");
  mu_check(result);
  
  /* char filename[256]; */
  /* sprintf(filename, "%s/%s.ppm", SCREENSHOT_FOLDER, __func__); */
  /* SaveImage(filename, &loaded); */
}

MU_TEST_SUITE(test_suite) {
  MU_RUN_TEST(test_check);
}

int main(int argc, char *argv[]) {
  MU_RUN_SUITE(test_suite);
  MU_REPORT();
  return MU_EXIT_CODE;
}
