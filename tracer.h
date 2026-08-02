#ifndef PPM_H
#define PPM_H

#include <stdio.h>
#include <stdlib.h>

// always assuming 256 color mode
typedef struct 
{
  unsigned char* buffer;
  int pxWidth;
  int pxHeight;

} PpmBuffer;

//todo: depricate
void save(unsigned char* buffer, int widthPixel, int heightPixel, char* fileName);
// theese are ok
void ppmbuffer_save(char* fileName, PpmBuffer* buffer);
bool ppmbuffer_load(char *path, PpmBuffer* result);
bool ppmbuffer_load_into(char *path, PpmBuffer* result);
bool ppmbuffer_same(PpmBuffer* abuffer, PpmBuffer* bbuffer);
bool ppmbuffer_compare_combine(PpmBuffer* abuffer, PpmBuffer* bbuffer, PpmBuffer* result);

void save(unsigned char* buffer, int widthPixel, int heightPixel, char* fileName){
  FILE *fptr;
  fptr = fopen(fileName, "w");

  // note: assuming path is ensured
  if( fptr == NULL ) {
    printf("Failed to fopen at:%s\n",fileName);
    return;
  }
  
  fprintf(fptr, "P3\n");
  fprintf(fptr, "%d %d\n", widthPixel, heightPixel);
  fprintf(fptr, "255\n");

  for( int j=0; j < heightPixel; j++){
    for ( int i=0; i< widthPixel; i++){
      int pixel = (i + widthPixel * j) *3;
      fprintf(fptr, "%d %d %d ", buffer[pixel +0], buffer[pixel+1], buffer[pixel+2]);
    }
  }
}

void ppmbuffer_save(char* fileName, PpmBuffer* buffer){
  save( buffer->buffer, buffer->pxWidth, buffer->pxHeight, fileName );
}

bool ppmbuffer_load(char *path, PpmBuffer* result){
    char* fullPath = path;

  // Open the file
  FILE* file = fopen(fullPath, "rb");
  if (!file) {
    printf("Error: Could not open file %s\n", fullPath);
    return false;
  }
  
  // header
  char format[3];
  int maxColor;
  
  // (P3 or P6)
  fscanf(file, "%2s", format);
  if (format[0] != 'P' || (format[1] != '3' && format[1] != '6')) {
    printf("Error: Not a valid PPM file\n");
    fclose(file);
    return false;
  }
  
  // Skip comments
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
    return false;
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
  return true;
}

bool ppmbuffer_load_into(char *path, PpmBuffer* result){
    char* fullPath = path;

  // Open the file
  FILE* file = fopen(fullPath, "rb");
  if (!file) {
    printf("Error: Could not open file %s\n", fullPath);
    return false;
  }
  
  // header
  char format[3];
  int maxColor;
  
  // (P3 or P6)
  fscanf(file, "%2s", format);
  if (format[0] != 'P' || (format[1] != '3' && format[1] != '6')) {
    printf("Error: Not a valid PPM file\n");
    fclose(file);
    return false;
  }
  
  // Skip comments
  char c = fgetc(file);
  while (c == '#') {
    while (c != '\n') c = fgetc(file);
    c = fgetc(file);
  }
  ungetc(c, file);
  
  // Read width and height
  int file_width;
  int file_height;
  fscanf(file, "%d %d", &file_width, &file_height);

  if( file_width != result->pxWidth || file_height != result->pxHeight){
    printf("Error: result/file width/height missmatch.\n");
    fclose(file);
    return false;
  }
  
  // Read max color value
  fscanf(file, "%d", &maxColor);

  if( result->buffer == NULL){
    printf("Error: result->buffer non allocaed.\n");
    fclose(file);
    return false;
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
  return true;
}

bool ppmbuffer_same(PpmBuffer* abuffer, PpmBuffer* bbuffer){
    if (!abuffer || !bbuffer) {
        printf("Error: NULL pointer passed to SameImage\n");
        return false;
    }
    
    if (!abuffer->buffer || !bbuffer->buffer) {
        printf("Error: One or both buffers are NULL\n");
        return false;
    }

    if (abuffer->pxWidth != bbuffer->pxWidth || 
        abuffer->pxHeight != bbuffer->pxHeight) {
        printf("Image dimensions differ: (%dx%d) vs (%dx%d)\n", 
               abuffer->pxWidth, abuffer->pxHeight, 
               bbuffer->pxWidth, bbuffer->pxHeight);
        return false;
    }
    
    int pixelCount = abuffer->pxWidth * abuffer->pxHeight * 3;
    for (int i = 0; i < pixelCount; i++) {
        if (abuffer->buffer[i] != bbuffer->buffer[i]) {
            printf("Images differ at pixel %d (byte %d)\n", i / 3, i % 3);
            return false;
        }
    }
    
    return true;
}

bool ppmbuffer_compare_combine(PpmBuffer* abuffer, PpmBuffer* bbuffer, PpmBuffer* result){
  if(abuffer->pxWidth != bbuffer->pxWidth ||
     abuffer->pxHeight != bbuffer->pxHeight )
    {
      printf("Not Implemented: compare buffers of different dimensions.");
      return false;
    }

  int width = abuffer->pxWidth;
  int height = abuffer->pxHeight;

  result->pxWidth = width * 3;
  result->pxHeight = height;
  result->buffer = (unsigned char*)malloc(result->pxWidth * result->pxHeight * 3); // 3 bytes per pixel (RGB)
  
  if (result->buffer == NULL) {
    return false; // Memory allocation failed
  }

  unsigned char* createdPixel;
  unsigned char* storedPixel;
  
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < (width * 3); i++) {
      if (i < width) {
        // left - stored 
        int srcIndex = (j * width + i) * 3;
        int dstIndex = (j * result->pxWidth + i) * 3;
        result->buffer[dstIndex] = abuffer->buffer[srcIndex];
        result->buffer[dstIndex + 1] = abuffer->buffer[srcIndex + 1];
        result->buffer[dstIndex + 2] = abuffer->buffer[srcIndex + 2];
      }
      else if (i < (width * 2)) {
        // compare result:
        int offset = i - width;
        int srcIndex = (j * width + offset) * 3;
        int dstIndex = (j * result->pxWidth + i) * 3;
        
        createdPixel = &bbuffer->buffer[srcIndex];
        storedPixel = &abuffer->buffer[srcIndex];
        
        if (createdPixel[0] == storedPixel[0] && 
            createdPixel[1] == storedPixel[1] && 
            createdPixel[2] == storedPixel[2]) {

	  int color_value = createdPixel[0] + createdPixel[1] + createdPixel[2];
	  int gray  = color_value / 3; 

          result->buffer[dstIndex] = gray;
	  result->buffer[dstIndex + 1] = gray;
	  result->buffer[dstIndex + 2] = gray;
	  
        } else {
          // hilight resulterence
          result->buffer[dstIndex] = 255;
          result->buffer[dstIndex + 1] = 0;
          result->buffer[dstIndex + 2] = 0;
        }
      }
      else {
	// bbuffer - right
        int offset = i - width * 2;
        int srcIndex = (j * width + offset) * 3;
        int dstIndex = (j * result->pxWidth + i) * 3;
        result->buffer[dstIndex] = bbuffer->buffer[srcIndex];
        result->buffer[dstIndex + 1] = bbuffer->buffer[srcIndex + 1];
        result->buffer[dstIndex + 2] = bbuffer->buffer[srcIndex + 2];
      }
    }
  }


  return true;
}


#endif

#ifndef V3_H
#define V3_H

#include <math.h>

typedef struct{
  float x;
  float y;
  float z;
} V3;

static inline
V3 v3_negate(V3 v){
  v.x=-v.x;
  v.y=-v.y;
  v.z=-v.z;
  return v;
}

static inline
float v3_dot(V3 a, V3 b){
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

static inline
float v3_len(V3 v){
  // TODO: check sqrtf
  return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

static inline
V3 v3_sub(V3 a, V3 b){
  return (V3){a.x-b.x, a.y - b.y, a.z-b.z};
}

#endif


#ifndef UTILITY_H
#define UTILITY_H

#define ARRAY_SIZE(arr)(sizeof(arr)/sizeof((arr)[0]))
#define BIG_NUMBER 9e10
#define EPSILON 0.001f

#endif


#ifndef RAY_H
#define RAY_H

#include <stddef.h>

typedef struct{
  V3 position;
  float radius;
  V3 color;
  float specular;
  float reflective;
  float _rr; // r*r pre-storing
} Sphere;

typedef struct{
  float t1;
  float t2;
  const Sphere* sphere;
} RaySphereIntersection;


V3 ReflectRay(V3 N, V3 R){
  V3 result = {0};
  float nDotl = v3_dot(R,N);
  result.x = 2*N.x*nDotl-R.x;
  result.y = 2*N.y*nDotl-R.y;
  result.z = 2*N.z*nDotl-R.z;
  return result;
}


RaySphereIntersection intersectRaySphere(const V3 O, const V3 D,const Sphere* restrict sphere){
  float rr = sphere->_rr;
  RaySphereIntersection result = {0};
  V3 sphereCenter = sphere->position;
  
  V3 CO = {O.x - sphereCenter.x, O.y - sphereCenter.y, O.z - sphereCenter.z };
  float a = v3_dot(D, D);
  float a2 = 2*a;
  float b = 2*v3_dot(CO, D);
  float c = v3_dot(CO, CO) - rr;

  float discriminant = b*b - 4*a*c;
  float sqrtDiscriminant = sqrtf(discriminant);
  if( discriminant < 0 )
    {
      return result;
    }

  float sqrtDiscriminantDivA2 = sqrtDiscriminant /a2;
  float minusBA2 = -b / a2;

  if( discriminant == 0 ){
    result.t1 = minusBA2 + sqrtDiscriminantDivA2;
    result.t2 = result.t1;
    return result;
  }

  if( discriminant > 0 ){
    result.t1 = minusBA2 + sqrtDiscriminantDivA2;
    result.t2 = minusBA2 - sqrtDiscriminantDivA2;
    return result;
  }

  return result;
}

RaySphereIntersection intersectRaySphereClosest(const V3 O, const V3 D, const float t_min,
						const float t_max,const  Sphere* restrict spheres,
						const int sphereCount){
  RaySphereIntersection  result = {0};

  float closest_t = BIG_NUMBER;
  const Sphere* closest_sphere = NULL;

  RaySphereIntersection hit;
  for ( int i=0; i< sphereCount; i++){
    const Sphere* current = spheres+i;
    hit = intersectRaySphere( O, D, current );

    if( hit.t1 > t_min && hit.t1 < t_max && hit.t1 < closest_t){
      closest_t = hit.t1;
      closest_sphere = current;
    }

    if( hit.t2 > t_min && hit.t2 < t_max && hit.t2 < closest_t){
      closest_t = hit.t2;
      closest_sphere = current;
    } 
  }

  result.t1 = closest_t;
  result.sphere = closest_sphere;
  return result;
}

#endif


#ifndef TRACER_H
#define TRACER_H

/* static const V3 DEFAULT_COLOR = {0,0,0}; */
#define DEFAULT_COLOR ((V3){0.0f,0.0f,0.0f})

typedef enum {LIGHT_AMBIENT, LIGHT_POINT, LIGHT_DIRECTIONAL} LIGHT_TYPE;

typedef struct{
  LIGHT_TYPE type;
  float intensity;
  V3 position;
} Light;

typedef struct{
  int width;
  int height;
  int size;
  unsigned char* start;
} Buffer;


// centered zero
// todo:  needs a better name
typedef struct{
  int top;
  int bot;
  int left;
  int right;
} Region;

float ComputeLighting(V3 P, V3 N, V3 View, float s,
		      const Sphere* spheres, int sphereCount,
		      Light* lights, int lightCount){
  float intensity = 0.0;
  V3 L;
  V3 Reflection;

  for( int i =0; i< lightCount; i++){
    Light* l = lights+i;

    if (l->type == LIGHT_AMBIENT){
      intensity += l->intensity;
      continue;
    }

    float t_max;
    if ( l->type == LIGHT_POINT ){
      L = v3_sub(l->position, P);
      t_max = 1;
    } else { // DIRECTIONAL
      L = l->position;
      t_max = BIG_NUMBER;
    }

    RaySphereIntersection intersection = intersectRaySphereClosest(P, L, EPSILON, t_max, spheres, sphereCount);
    if( intersection.sphere != NULL ){
      continue;
    }

    
    // DIFFUSE
    float nDotl = v3_dot( N, L);
    if ( nDotl > 0 ){
      intensity += l->intensity * nDotl / (v3_len(N) * v3_len(L)) ;
    }

    // SPECULAR
    if ( s != -1){

      Reflection = ReflectRay(N,L);
      
      float rDotV = v3_dot( Reflection, View);
      if (rDotV >0){
	intensity += l->intensity * pow( rDotV / (v3_len(Reflection) * v3_len(View)), s );
      }
    }
    
  }
  return intensity;
}

// todo: scene description
 V3 traceRay( V3 O, V3 D, float t_min, float t_max, int recursion_depth,
	     const Sphere* spheres, int sphereCount,
	     Light* lights, int lightCount){
  float closest_t = BIG_NUMBER;
  const Sphere *closestSphere = NULL;
  
  RaySphereIntersection intersection = intersectRaySphereClosest(O, D, t_min, t_max, spheres, sphereCount);
  closestSphere = intersection.sphere;
  closest_t = intersection.t1;


  if( closestSphere == NULL ){
    return DEFAULT_COLOR;
  }
  
  /* // no light */
  /* if(closestSphere!=NULL){ */
  /*   return closestSphere->color; */
  /* } */
  
  // Lit
  V3 P;
  P.x = O.x + closest_t * D.x;
  P.y = O.y + closest_t * D.y;
  P.z = O.z + closest_t * D.z;

  V3 N;
  N.x = P.x - closestSphere->position.x;
  N.y = P.y - closestSphere->position.y;
  N.z = P.z - closestSphere->position.z;;

  V3 local_color = closestSphere->color;

  V3 v = v3_negate(D);
  // v from object to camera = -D from camera to object
  float light = ComputeLighting(P,N,v,closestSphere->specular, spheres, sphereCount, lights, lightCount);

  local_color.x *= light;
  local_color.y *= light;
  local_color.z *= light;

  float reflective = closestSphere->reflective;
  if ( recursion_depth <= 0 || reflective <=0 ){
    return local_color;
  }

  // -D
  V3 R = ReflectRay(N,v);
  V3 reflected_color = traceRay(P, R, EPSILON, BIG_NUMBER, recursion_depth-1, spheres, sphereCount, lights, lightCount);
  V3 result;
  result.x = local_color.x*(1-reflective) + reflected_color.x*reflective;
  result.y = local_color.y*(1-reflective) + reflected_color.y*reflective;
  result.z = local_color.z*(1-reflective) + reflected_color.z*reflective;
  
  return result;
}

void setPixelTexture(float x, float y, V3 color, Buffer *buffer) {
  int byteOffset = (x + buffer->width * y) * 3;
  // potential branchless operation
  // clamp [0, 255]
  buffer->start[byteOffset + 0] =
      (unsigned char)fmaxf(0.0, fminf(color.x, 255.0));
  buffer->start[byteOffset + 1] =
      (unsigned char)fmaxf(0.0, fminf(color.y, 255.0));
  buffer->start[byteOffset + 2] =
      (unsigned char)fmaxf(0.0, fminf(color.z, 255.0));
}


void setPixelCanvas(float x, float y, V3 color, Buffer *buffer) {
  // in x - in Canvas space - w/2 to w/2
  // out x - texture coords  0 to w

  // in y - in Canvas space  [h/2 (top) , -h/2(bot)]
  // by - HEIGHT/2   [ 0 , -h]
  // and *-1 [0, h] according to texture space
  setPixelTexture(x + (buffer->width) / 2, -(y - (buffer->height) / 2), color,
                  buffer);
}


void fillRegion
( V3 origin, V3 cameraDirection, Region region, V3 viewportSize,float projectionPlane,
		 Buffer buffer,
		 float t_min, float t_max, int recursion_depth,
		 Sphere* spheres, int sphereCount,
		 Light* lights, int lightCount){
  int topEdge = region.top;
  int bottomEdge = region.bot;

  int leftEdge = region.left;
  int righEdge = region.right;

  V3 direction = {0};
  V3 color;

  int width = buffer.width;
  int height = buffer.height;

  unsigned char* bufferStart = buffer.start;

  
  // Create a right vector (perpendicular to camera direction)
  // Assuming Y is up
  V3 up = {0, 1, 0};
  V3 right;

  // TODO: re-review
  // cross
  right.x = up.y * cameraDirection.z - up.z * cameraDirection.y;
  right.y = up.z * cameraDirection.x - up.x * cameraDirection.z;
  right.z = up.x * cameraDirection.y - up.y * cameraDirection.x;
  
  float rightLen = sqrtf(right.x*right.x + right.y*right.y + right.z*right.z);
  if (rightLen > 0) {
    right.x /= rightLen;
    right.y /= rightLen;
    right.z /= rightLen;
  }
  
  V3 actualUp;
  actualUp.x = cameraDirection.y * right.z - cameraDirection.z * right.y;
  actualUp.y = cameraDirection.z * right.x - cameraDirection.x * right.z;
  actualUp.z = cameraDirection.x * right.y - cameraDirection.y * right.x;
  

  for( int i=0; i< sphereCount; i++){
    float r = spheres[i].radius;
    spheres[i]._rr = r*r;
  }
  
  // NOTE:
  // tracer relies or rba pixel format 3 components
  // win expects 4 components
  // will change for other platforms  ?!
  int targetBufferColorComponents = 3;
  
  for (int y = topEdge; y > bottomEdge; y--) {
    for (int x = leftEdge; x < righEdge; x++) {
      /* direction = canvasToViewport(x, y, buffer.width, buffer.height, */
      /*                              viewportSize, projectionPlane); */

      // x and y
      //      height /2
      //-width/2      width/2
      //     -height /2

      // vieport size currently 1, 1
      // width and height are buffer size

      // that makes direction
      //      0.5
      //-0.5      0.5
      //     -0.5

      // todo: review
      // These are offsets from the center of the viewport
      float viewportX = x * viewportSize.x / width;
      float viewportY = y * viewportSize.y / height;
      
      // Transform viewport coordinates into world space using camera orientation
      direction.x = cameraDirection.x * projectionPlane + right.x * viewportX + actualUp.x * viewportY;
      direction.y = cameraDirection.y * projectionPlane + right.y * viewportX + actualUp.y * viewportY;
      direction.z = cameraDirection.z * projectionPlane + right.z * viewportX + actualUp.z * viewportY;
      
      // Normalize direction
      float dirLen = sqrtf(direction.x*direction.x + direction.y*direction.y + direction.z*direction.z);
      if (dirLen > 0) {
        direction.x /= dirLen;
        direction.y /= dirLen;
        direction.z /= dirLen;
      }
      
      color = traceRay(origin, direction, t_min, t_max, recursion_depth,
		       spheres,  sphereCount,
		       lights, lightCount);

      /* setPixelCanvas */
      // in x - in Canvas space - w/2 to w/2
      // out x - texture coords  0 to w
      // in y - in Canvas space  [h/2 (top) , -h/2(bot)]
      // by - HEIGHT/2   [ 0 , -h]
      // and *-1 [0, h] according to texture space


      // todo: clenup this
      int columnOffsetPx = x + width / 2;
      int rowOffsetPx =  -(y - height / 2);
      
      /* /\* setPixelTexture *\/ */
      int byteOffset = (width * rowOffsetPx + columnOffsetPx) * targetBufferColorComponents;
      /* // potential branchless operation */
      /* // clamp [0, 255] */
      bufferStart[byteOffset + 0] =
	(unsigned char)fmaxf(0.0, fminf(color.x, 255.0));
      bufferStart[byteOffset + 1] =
	(unsigned char)fmaxf(0.0, fminf(color.y, 255.0));
      bufferStart[byteOffset + 2] =
	(unsigned char)fmaxf(0.0, fminf(color.z, 255.0));
      
    }
  }
}

// todo: unite or better pack
// win method for specific target buffer packing
// 4 components, different ordering
// 
void fillRegionWin( V3 origin, V3 cameraDirection, Region region, V3 viewportSize,float projectionPlane,
		 Buffer buffer,
		 float t_min, float t_max, int recursion_depth,
		 Sphere* spheres, int sphereCount,
		 Light* lights, int lightCount){

  int topEdge = region.top;
  int bottomEdge = region.bot;

  int leftEdge = region.left;
  int righEdge = region.right;

  V3 direction = {0};
  V3 color;

  int width = buffer.width;
  int height = buffer.height;

  unsigned char* bufferStart = buffer.start;


  // Create a right vector (perpendicular to camera direction)
  // Assuming Y is up
  V3 up = {0, 1, 0};
  V3 right;

  // TODO: re-review
  // cross
  right.x = up.y * cameraDirection.z - up.z * cameraDirection.y;
  right.y = up.z * cameraDirection.x - up.x * cameraDirection.z;
  right.z = up.x * cameraDirection.y - up.y * cameraDirection.x;
  
  float rightLen = sqrtf(right.x*right.x + right.y*right.y + right.z*right.z);
  if (rightLen > 0) {
    right.x /= rightLen;
    right.y /= rightLen;
    right.z /= rightLen;
  }
  
  V3 actualUp;
  actualUp.x = cameraDirection.y * right.z - cameraDirection.z * right.y;
  actualUp.y = cameraDirection.z * right.x - cameraDirection.x * right.z;
  actualUp.z = cameraDirection.x * right.y - cameraDirection.y * right.x;
  
  for( int i=0; i< sphereCount; i++){
    float r = spheres[i].radius;
    spheres[i]._rr = r*r;
  }
  
  // NOTE:
  // tracer relies or rba pixel format 3 components
  // win expects 4 components
  // win BGRA
  int targetBufferColorComponents = 4;
  
  for (int y = topEdge; y > bottomEdge; y--) {
    for (int x = leftEdge; x < righEdge; x++) {

      // x and y
      //      height /2
      //-width/2      width/2
      //     -height /2

      // vieport size currently 1, 1
      // width and height are buffer size

      // that makes direction
      //      0.5
      //-0.5      0.5
      //     -0.5

      // todo: review
      // These are offsets from the center of the viewport
      float viewportX = x * viewportSize.x / width;
      float viewportY = y * viewportSize.y / height;
      
      // Transform viewport coordinates into world space using camera orientation
      direction.x = cameraDirection.x * projectionPlane + right.x * viewportX + actualUp.x * viewportY;
      direction.y = cameraDirection.y * projectionPlane + right.y * viewportX + actualUp.y * viewportY;
      direction.z = cameraDirection.z * projectionPlane + right.z * viewportX + actualUp.z * viewportY;
      
      // Normalize direction
      float dirLen = sqrtf(direction.x*direction.x + direction.y*direction.y + direction.z*direction.z);
      if (dirLen > 0) {
        direction.x /= dirLen;
        direction.y /= dirLen;
        direction.z /= dirLen;
      }
      
      
      color = traceRay(origin, direction, t_min, t_max, recursion_depth,
		       spheres,  sphereCount,
		       lights, lightCount);

      /* setPixelCanvas */
      // in x - in Canvas space - w/2 to w/2
      // out x - texture coords  0 to w
      // in y - in Canvas space  [h/2 (top) , -h/2(bot)]
      // by - HEIGHT/2   [ 0 , -h]
      // and *-1 [0, h] according to texture space

      // todo: clenup this
      int columnOffsetPx = x + width / 2;
      int rowOffsetPx =  -(y - height / 2);
      
      /* /\* setPixelTexture *\/ */
      int byteOffset = (width * rowOffsetPx + columnOffsetPx) * targetBufferColorComponents;
      /* // potential branchless operation */
      /* // clamp [0, 255] */
      bufferStart[byteOffset + 0] =  // BLUE
	(unsigned char)fmaxf(0.0, fminf(color.z, 255.0));
      bufferStart[byteOffset + 1] = // GREEN
	(unsigned char)fmaxf(0.0, fminf(color.y, 255.0));
      bufferStart[byteOffset + 2] = // RED
	(unsigned char)fmaxf(0.0, fminf(color.x, 255.0));
      bufferStart[byteOffset + 3] =
      (unsigned char)255.0;
      
    }
  }
}





#endif
