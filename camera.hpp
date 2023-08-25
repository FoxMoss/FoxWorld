#ifndef CAMERA_H
#define CAMERA_H

#include "raylib.h"
#include "raymath.h"
#include <thread>
#include <vector>

#define EP 1
#define MAX 100
#define SCALE 5
#define MAX_THREADS 5

class RaycastRay {
private:
public:
  Vector3 direction;
  Vector3 directionComputed;
  Vector3 origin;
  Vector3 rotation;

  RaycastRay(Vector3 cDirection, Vector3 cOrigin);
  ~RaycastRay();
  Color GetColor();
};
class FoxPlane {
public:
  FoxPlane(Vector3 normal, Vector3 origin);
  ~FoxPlane();
  float IntersectsRay(RaycastRay ray);

  Vector3 origin;
  Vector3 normal;

private:
};
class FoxTri {
public:
  FoxTri(Vector3 A, Vector3 B, Vector3 C);
  ~FoxTri();
  float IntersectsRay(RaycastRay ray, float maxDist);
  Vector3 normal;

  Vector3 A;
  Vector3 B;
  Vector3 C;

private:
  FoxPlane *raycastPlane;
  // Pre-Calc
  Vector3 BASub;
  Vector3 CBSub;
  Vector3 ACSub;
};

struct FoxModel {
  FoxTri **tris;
  int size;
};
class FoxCamera {
private:
  int width;
  int height;
  int vmax;
  int aspectRatio;
  pthread_t threads[MAX_THREADS];
  // std::vector<FoxModel> models;

  RaycastRay **rays;

public:
  FoxCamera(int cWidth, int cHeight);
  ~FoxCamera();
  void Render(Image *image);
  void RenderChunk(Image *image, int chunk);
  static void *ThreadHelper(void *context);
  Vector3 position;
  Vector3 rotation;
};

FoxModel makeCube(Vector3 position, Vector3 scale);
FoxModel makeTorus(Vector3 position, Vector3 scale);
void setupModels();
FoxModel readTris();
FoxTri **vertToTri(float *vert, int size, Vector3 position, Vector3 scale);

inline std::vector<FoxModel> models;
void proccessPixel(Image *image, int x, int y, RaycastRay *ray);

typedef struct ChunkInfo {
  FoxCamera *camera;
  Image *image;
  int chunk;
} ChunkInfo;
#endif
