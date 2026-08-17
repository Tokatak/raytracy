
#ifndef PPM_H
#define PPM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// always assuming 256 color mode
typedef struct 
{
  unsigned char* buffer;
  int pxWidth;
  int pxHeight;

} PpmBuffer;

void ppmbuffer_save(const char* restrict fileName,const PpmBuffer* restrict buffer);
bool ppmbuffer_load(const char* restrict path,PpmBuffer* restrict result);
bool ppmbuffer_load_into(char* restrict path, PpmBuffer* restrict result);
bool ppmbuffer_same(PpmBuffer* restrict abuffer, PpmBuffer* restrict bbuffer);
bool ppmbuffer_compare_combine(PpmBuffer* restrict abuffer, PpmBuffer* restrict bbuffer, PpmBuffer* restrict result);


void ppmbuffer_save(const char* restrict fileName,const PpmBuffer* restrict ppm_buffer){
  unsigned char* buffer = ppm_buffer->buffer;
  int widthPixel = ppm_buffer->pxWidth;
  int heightPixel =  ppm_buffer->pxHeight;
  
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

bool ppmbuffer_load(const char* restrict path,PpmBuffer* restrict result){
  const char* fullPath = path;

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

  if (result->buffer == NULL) {
    return false; // Memory was not provided
  }

  int width = abuffer->pxWidth;
  int height = abuffer->pxHeight;

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

// ffast-math ?
#define _USE_MATH_DEFINES
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
float v3_dot(const V3 a,const V3 b){
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

static inline
float v3_len(const V3 v){
  // TODO: check sqrtf
  return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

static inline
V3 v3_sub(const V3 a,const V3 b){
  return (V3){a.x-b.x, a.y - b.y, a.z-b.z};
}

#endif

#ifndef COLOR_BUFFER_H
#define COLOR_BUFFER_H
typedef struct{
  float_t* x;
  float_t* y;
  float_t* z;
  size_t count;
} ColorBuffer;
#endif

#ifndef SPHERE_BUFFER_H
#define SPHERE_BUFFER_H
typedef struct{ 
  float_t* x;
  float_t* y;
  float_t* z;
  float_t* radius;
  float_t* r;
  float_t* g;
  float_t* b;
  float_t* specular;
  float_t* reflective;
  float_t* rr;

  size_t count;  
} SphereBuffer;
#endif

#ifndef LIGHT_BUFFER_H
#define LIGHT_BUFFER_H
typedef struct{
  float_t* ambient_intensity;
  size_t ambient_count;

  float_t* point_x;
  float_t* point_y;
  float_t* point_z;
  float_t* point_intensity;
  size_t point_count;
 
  float_t* dir_x;
  float_t* dir_y;
  float_t* dir_z;
  float* dir_intensity;
  size_t dir_count;
  
} LightBuffer;
#endif

#ifndef DIRECTION_BUFFER_H
#define DIRECTION_BUFFER_H
typedef struct{
  float_t* x;
  float_t* y;
  float_t* z;
  size_t count;
} DirectionBuffer;
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
  float _rr; // r*r pre-storing //todo: validate
} Sphere;

typedef struct{
  float t1;
  float t2;
  const Sphere* sphere;
} RaySphereIntersection;

static inline void ReflectRay(const V3 N,const V3 R,V3* const restrict result);
RaySphereIntersection intersectRaySphere(const V3 O, const V3 D,const Sphere* restrict sphere);
RaySphereIntersection intersectRaySphereClosest(const V3 O, const V3 D, const float t_min,
						const float t_max,const  Sphere* restrict spheres,
						const int sphereCount);

static inline void ReflectRay(const V3 N,const V3 R,V3* const restrict result){
  const float twoNDotl = 2*v3_dot(R,N);
  result->x = twoNDotl*N.x-R.x;
  result->y = twoNDotl*N.y-R.y;
  result->z = twoNDotl*N.z-R.z;
}

RaySphereIntersection intersectRaySphere(const V3 O, const V3 D,const Sphere* restrict sphere){
  RaySphereIntersection result = {0};
  float rr = sphere->_rr;
  const float cx = sphere->position.x;
  const float cy = sphere->position.y;
  const float cz = sphere->position.z;
  const float ocx = O.x - cx;
  const float ocy = O.y - cy;
  const float ocz = O.z - cz;    
  const float dx = D.x, dy = D.y, dz = D.z;
  const float a = dx*dx + dy*dy + dz*dz;
  const float b = 2.0f * (ocx*dx + ocy*dy + ocz*dz);
  const float c = ocx*ocx + ocy*ocy + ocz*ocz - rr;
  
  float discriminant = b*b - 4*a*c;
  if( discriminant < 0 )
    {
      return result;
    }  
  const float sqrtDiscriminant = sqrtf(discriminant);
  const float inv_2a = 1.0f / ( 2.0f * a);
  const float center = -b * inv_2a;
  const float offset = sqrtDiscriminant * inv_2a;
  result.t1 = center + offset;
  result.t2 = center - offset;
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

typedef struct {
    int components;
    int alpha_offset;  // -1 if no alpha
    int r_offset;
    int g_offset;
    int b_offset;
} PixelLayout;

typedef struct {
  V3 position;
  V3 direction;
  V3 viewportSize;
  float projectionPlane;
} Camera;

static const PixelLayout PIXEL_LAYOUT_RGB = {3, -1, 0, 1, 2};
static const PixelLayout PIXEL_LAYOUT_BGR = {3, -1, 2, 1, 0};
static const PixelLayout PIXEL_LAYOUT_RGBA = {4, 3, 0, 1, 2};
static const PixelLayout PIXEL_LAYOUT_BGRA = {4, 3, 2, 1, 0};


float ComputeLighting(V3 P, V3 N, V3 View, float s,
		      const Sphere* spheres, int sphereCount,
		      Light* lights, int lightCount);

V3 traceRay( V3 O, V3 D, float t_min, float t_max, int recursion_depth,
	     const Sphere* spheres, int sphereCount,
	     Light* lights, int lightCount);

void traceRayBatch(const V3 Origin,
		   const DirectionBuffer directionBuffer,const size_t startAt,const size_t batchSize,
		   const float t_min,const float t_max,const int recursion_depth,
		   const SphereBuffer spheres,
		   const LightBuffer lights,
		   ColorBuffer* const result);


void setPixelTexture(float x, float y, V3 color, Buffer *buffer);
void setPixelCanvas(float x, float y, V3 color, Buffer *buffer);
void fillRegion
( Region region, Camera camera,
  Buffer buffer,PixelLayout layout,
  float t_min, float t_max, int recursion_depth,
  Sphere* spheres, int sphereCount,
  Light* lights, int lightCount);

float ComputeLighting(V3 P, V3 N, V3 View, float s,
		      const Sphere* spheres, int sphereCount,
		      Light* lights, int lightCount){
  float intensity = 0.0;
  V3 L;
  V3 Reflection;

  float N_len = v3_len(N);
  float View_len = v3_len(View);

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
      intensity += l->intensity * nDotl / (N_len * v3_len(L)) ;
    }

    // SPECULAR
    if ( s != -1){
      ReflectRay(N,L,&Reflection);
      
      float rDotV = v3_dot( Reflection, View);
      if (rDotV >0){
	intensity += l->intensity * powf( rDotV / (v3_len(Reflection) * View_len), s );
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
  
  // no light
  if(lightCount == 0){
    return closestSphere->color;
  }
  
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
  V3 R = {0}; 
  ReflectRay(N,v,&R);
  V3 reflected_color = traceRay(P, R, EPSILON, BIG_NUMBER, recursion_depth-1, spheres, sphereCount, lights, lightCount);
  V3 result;
  result.x = local_color.x*(1-reflective) + reflected_color.x*reflective;
  result.y = local_color.y*(1-reflective) + reflected_color.y*reflective;
  result.z = local_color.z*(1-reflective) + reflected_color.z*reflective;
  
  return result;
}


void traceRayBatch(const V3 Origin,
		   const DirectionBuffer directionBuffer,const size_t startAt,const size_t batchSize,
		   const float t_min,const float t_max,const int recursion_depth,
		   const SphereBuffer spheres,
		   const LightBuffer lights,
		   ColorBuffer* const result){

  (void)Origin;
  (void)directionBuffer;
  (void)startAt;
  (void)batchSize;
  (void)t_min;
  (void)t_max;
  (void)recursion_depth;
  (void)spheres;
  (void)lights;
  (void)result;
  
  

  /* const float closest_t = BIG_NUMBER; */
  /* const Sphere *closestSphere = NULL; */
  
  /* RaySphereIntersection intersection = intersectRaySphereClosest(O, D, t_min, t_max, spheres, sphereCount); */
  /* closestSphere = intersection.sphere; */
  /* closest_t = intersection.t1; */


  /* if( closestSphere == NULL ){ */
  /*   return DEFAULT_COLOR; */
  /* } */
  
  /* // no light */
  /* if(lightCount == 0){ */
  /*   return closestSphere->color; */
  /* } */
  
  /* // Lit */
  /* V3 P; */
  /* P.x = O.x + closest_t * D.x; */
  /* P.y = O.y + closest_t * D.y; */
  /* P.z = O.z + closest_t * D.z; */

  /* V3 N; */
  /* N.x = P.x - closestSphere->position.x; */
  /* N.y = P.y - closestSphere->position.y; */
  /* N.z = P.z - closestSphere->position.z;; */

  /* V3 local_color = closestSphere->color; */

  /* V3 v = v3_negate(D); */
  /* // v from object to camera = -D from camera to object */
  /* float light = ComputeLighting(P,N,v,closestSphere->specular, spheres, sphereCount, lights, lightCount); */

  /* local_color.x *= light; */
  /* local_color.y *= light; */
  /* local_color.z *= light; */

  /* float reflective = closestSphere->reflective; */
  /* if ( recursion_depth <= 0 || reflective <=0 ){ */
  /*   return local_color; */
  /* } */

  /* // -D */
  /* V3 R = {0};  */
  /* ReflectRay(N,v,&R); */
  /* V3 reflected_color = traceRay(P, R, EPSILON, BIG_NUMBER, recursion_depth-1, spheres, sphereCount, lights, lightCount); */
  /* V3 result; */
  /* result.x = local_color.x*(1-reflective) + reflected_color.x*reflective; */
  /* result.y = local_color.y*(1-reflective) + reflected_color.y*reflective; */
  /* result.z = local_color.z*(1-reflective) + reflected_color.z*reflective; */
  
  /* return result; */

  return;
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
( Region region, Camera camera,
  Buffer buffer,PixelLayout layout,
  float t_min, float t_max, int recursion_depth,
  Sphere* spheres, int sphereCount,
  Light* lights, int lightCount)
{
  V3 origin = camera.position;
  V3 cameraDirection = camera.direction;
  V3 viewportSize = camera.viewportSize;
  float projectionPlane = camera.projectionPlane;
   
  int topEdge = region.top;
  int bottomEdge = region.bot;

  int leftEdge = region.left;
  int righEdge = region.right;

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

  LightBuffer lightBuffer ={0};

  size_t ambient = 0, point = 0, dir = 0;
  for (int i = 0; i < lightCount; i++) {
    switch (lights[i].type) {
    case LIGHT_AMBIENT: ambient++; break;
    case LIGHT_POINT: point++; break;
    case LIGHT_DIRECTIONAL: dir++; break;
    }
  }

  lightBuffer.ambient_count = ambient;
  lightBuffer.ambient_intensity = malloc(ambient*sizeof(float));

  lightBuffer.point_count = point;
  lightBuffer.point_x = malloc(point*sizeof(float));
  lightBuffer.point_y = malloc(point*sizeof(float));
  lightBuffer.point_z = malloc(point*sizeof(float));
  lightBuffer.point_intensity = malloc(point*sizeof(float));

  lightBuffer.dir_count = dir;
  lightBuffer.dir_x = malloc(dir*sizeof(float));
  lightBuffer.dir_y = malloc(dir*sizeof(float));
  lightBuffer.dir_z = malloc(dir*sizeof(float));
  lightBuffer.dir_intensity = malloc(dir*sizeof(float)); 

  size_t ambient_idx = 0, point_idx = 0, dir_idx = 0;
  for (int i=0; i< lightCount; i++){
    const Light l = lights[i];
    
    switch (l.type){
    case LIGHT_AMBIENT: {
      lightBuffer.ambient_intensity[ambient_idx] = l.intensity;
      ambient_idx ++;
      break;
    }
    case LIGHT_POINT: {
      lightBuffer.point_intensity[point_idx] = l.intensity;
      lightBuffer.point_x[point_idx] = l.position.x;
      lightBuffer.point_y[point_idx] = l.position.y;
      lightBuffer.point_z[point_idx] = l.position.z;
      point_idx ++;
      break;
    }
    case LIGHT_DIRECTIONAL: {      
      lightBuffer.dir_x[dir_idx] = l.position.x;
      lightBuffer.dir_y[dir_idx] = l.position.y;
      lightBuffer.dir_z[dir_idx] = l.position.z;
      lightBuffer.dir_intensity[dir_idx] = l.intensity;
      dir_idx ++;
      break;
    }
    default: break;
    }
  }

  // todo: remove
  for( int i=0; i< sphereCount; i++){
    float r = spheres[i].radius;
    spheres[i]._rr = r*r;
  }
  SphereBuffer sphereBuffer ={0};
  sphereBuffer.x = malloc(sizeof(float_t)*sphereCount);
  sphereBuffer.y = malloc(sizeof(float_t)*sphereCount);
  sphereBuffer.z = malloc(sizeof(float_t)*sphereCount);
  sphereBuffer.radius = malloc(sizeof(float_t)*sphereCount);
  sphereBuffer.r = malloc(sizeof(float_t)*sphereCount);
  sphereBuffer.g = malloc(sizeof(float_t)*sphereCount);
  sphereBuffer.b = malloc(sizeof(float_t)*sphereCount);
  sphereBuffer.specular = malloc(sizeof(float_t)*sphereCount);
  sphereBuffer.reflective = malloc(sizeof(float_t)*sphereCount);
  sphereBuffer.rr = malloc(sizeof(float_t)*sphereCount);
  sphereBuffer.count = sphereCount;
  for(int i =0; i< sphereCount; i++ ){
    Sphere s = spheres[i];

    sphereBuffer.x[i] = s.position.x;
    sphereBuffer.y[i] = s.position.y;
    sphereBuffer.z[i] = s.position.z;

    sphereBuffer.radius[i] = s.radius;

    const float r = s.radius;
    sphereBuffer.rr[i] = r*r;
    
    sphereBuffer.r[i] = s.color.x;
    sphereBuffer.g[i] = s.color.y;
    sphereBuffer.b[i] = s.color.z;

    sphereBuffer.specular[i] = s.specular;
    sphereBuffer.reflective[i] = s.reflective;    
  }


  int targetBufferColorComponents = layout.components;
  float normWidth = viewportSize.x / width;
  float normHeight = viewportSize.y / height;
  
  float cameraDirectionXprojectionPlaneX = cameraDirection.x * projectionPlane;
  float cameraDirectionXprojectionPlaneY = cameraDirection.y * projectionPlane;
  float cameraDirectionXprojectionPlaneZ = cameraDirection.z * projectionPlane;

  /* int halfWidth = width /2; */
  /* int halfHeight = height/2; */

  // 640*480 307'200
  int expectedPixelCount = ((abs(topEdge) + abs(bottomEdge)) * (abs(leftEdge)+abs(righEdge) ) ) ;

  DirectionBuffer directionsBuffer = {0};
  directionsBuffer.count = expectedPixelCount;
  directionsBuffer.x = malloc(sizeof(float_t)*expectedPixelCount);
  directionsBuffer.y = malloc(sizeof(float_t)*expectedPixelCount);
  directionsBuffer.z = malloc(sizeof(float_t)*expectedPixelCount);

  // todo: consider faield allocation 
  //todo: consider alligned alocations
/* #ifdef __AVX2__ */
/* #define SIMD_ALIGNMENT 32  // AVX2 needs 32-byte alignment */
/* #elif __SSE__ */
/* #define SIMD_ALIGNMENT 16  // SSE needs 16-byte alignment */
/* #else */
/* #define SIMD_ALIGNMENT 8   // Default */
/* #endif */

/*   // Allocate aligned memory for faster SIMD loads */
/*   directionsBuffer.x = (float_t*)aligned_alloc(SIMD_ALIGNMENT,  */
/* 					 sizeof(float_t) * pixelCount); */
/*   directionsBuffer.y = (float_t*)aligned_alloc(SIMD_ALIGNMENT,  */
/* 					 sizeof(float_t) * pixelCount); */
/*   directionsBuffer.z = (float_t*)aligned_alloc(SIMD_ALIGNMENT,  */
/* 					 sizeof(float_t) * pixelCount); */

  //todo: consider padding
  
  for (int screeenY = topEdge, pixelIndex = 0; screeenY > bottomEdge; screeenY--) {
    const float viewportY = screeenY * normHeight;
    const float baseX = cameraDirectionXprojectionPlaneX + actualUp.x * viewportY;
    const float baseY = cameraDirectionXprojectionPlaneY + actualUp.y * viewportY;
    const float baseZ = cameraDirectionXprojectionPlaneZ + actualUp.z * viewportY;
    
    for (int screenX = leftEdge; screenX < righEdge; screenX++) {
      const float viewportX = screenX * normWidth;      
      float x = baseX + right.x * viewportX;
      float y = baseY + right.y * viewportX;
      float z = baseZ + right.z * viewportX;

      float dirLen = sqrtf(x*x + y*y + z*z);
      float invDirLen = 1/dirLen;
      if (dirLen > 0) {
        x *= invDirLen;
        y *= invDirLen;
        z *= invDirLen;
      }

      directionsBuffer.x[pixelIndex] = x;
      directionsBuffer.y[pixelIndex] = y;
      directionsBuffer.z[pixelIndex] = z;
      pixelIndex++;
    }
  }

  size_t pixelCount = directionsBuffer.count; 
  for ( size_t index = 0; index< pixelCount; index++){
    
    // TODO: conitnue here
    // LightBuffer, SphereBuffer, directionsBuffer
    color = traceRay(origin,
		     (V3){directionsBuffer.x[index], directionsBuffer.y[index], directionsBuffer.z[index]},
		     t_min, t_max, recursion_depth,
		     spheres,  sphereCount,
		     lights, lightCount);

      
    const int byteOffset = index * targetBufferColorComponents;
    bufferStart[byteOffset + layout.r_offset] =
      (unsigned char)(color.x > 255.0f ? 255.0f : (color.x < 0.0f ? 0.0f : color.x)); 
    bufferStart[byteOffset + layout.g_offset] =
      (unsigned char)(color.y > 255.0f ? 255.0f : (color.y < 0.0f ? 0.0f : color.y)); 
    bufferStart[byteOffset + layout.b_offset] =
      (unsigned char)(color.z > 255.0f ? 255.0f : (color.z < 0.0f ? 0.0f : color.z));

    if ( targetBufferColorComponents >3 ) {
      bufferStart[byteOffset + layout.alpha_offset] = (unsigned char)255.0;
    }
  }

  // todo: consider using pthreads
  // for omp paste -fopenmp in gcc compile line
  // #pragma omp parallel for

  // todo:
  //free directionsBuffer
  free(directionsBuffer.x);
  free(directionsBuffer.y);
  free(directionsBuffer.z);
  
  // todo: prettify
  //free sphere buffer
  free(sphereBuffer.x);
  free(sphereBuffer.y);
  free(sphereBuffer.z);
  free(sphereBuffer.radius);
  free(sphereBuffer.r);
  free(sphereBuffer.g);
  free(sphereBuffer.b);
  free(sphereBuffer.specular);
  free(sphereBuffer.reflective);
  free(sphereBuffer.rr);

  //light
  free(lightBuffer.ambient_intensity);

  free(lightBuffer.point_x);
  free(lightBuffer.point_y);
  free(lightBuffer.point_z);
  free(lightBuffer.point_intensity);

  free(lightBuffer.dir_x);
  free(lightBuffer.dir_y);
  free(lightBuffer.dir_z);
  free(lightBuffer.dir_intensity);
}

#endif
