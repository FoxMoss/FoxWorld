#pragma once

#include "raylib.h"
#include "raymath.h"
#include <thread>
#include <vector>

#define EP 1
#define MAX 100
#define SCALE 5
#define MAX_THREADS 5
#define FOV 2.5

typedef struct FoxModel FoxModel;
class RaycastRay {
private:
public:
  Vector3 direction;
  Vector3 directionComputed;
  Vector3 origin;
  Vector3 rotation;

  RaycastRay(Vector3 cDirection, Vector3 cOrigin);
  ~RaycastRay();
  Color GetColor(std::vector<FoxModel> *models);
};
class FoxPlane {
public:
  FoxPlane(Vector3 normal, Vector3 origin);
  ~FoxPlane();
  float IntersectsRay(RaycastRay ray);
  float DistanceToPoint(Vector3 point);
  Vector3 PointForDistance(float dist, Vector3 point);

  Vector3 origin;
  Vector3 normal;

private:
};
class FoxTri {
public:
  FoxTri(Vector3 A, Vector3 B, Vector3 C);
  ~FoxTri();
  float IntersectsRay(RaycastRay ray, float maxDist);
  float IntersectsSphere(Vector3 point, float radius);
  float PointInside(Vector3 point, float scalar);
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
  int hue;
};
class FoxCamera {
private:
  int width;
  int height;
  int vmax;
  int aspectRatio;
  pthread_t threads[MAX_THREADS];
  RaycastRay **rays;

public:
  FoxCamera(int cWidth, int cHeight);
  ~FoxCamera();
  void Render(Image *image, std::vector<FoxModel> *models);
  void RenderChunk(Image *image, int chunk, std::vector<FoxModel> *models);
  static void *ThreadHelper(void *context);
  Vector3 position;
  Vector3 rotation;
};

FoxModel makeCube(Vector3 position, Vector3 scale);
FoxModel makeTorus(Vector3 position, Vector3 scale);
FoxModel readTris();
FoxTri **vertToTri(float *vert, int size, Vector3 position, Vector3 scale);

void proccessPixel(Image *image, int x, int y, RaycastRay *ray,
                   std::vector<FoxModel> *models);

typedef struct ChunkInfo {
  FoxCamera *camera;
  Image *image;
  int chunk;
  std::vector<FoxModel> *models;
} ChunkInfo;
