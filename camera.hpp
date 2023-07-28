#ifndef CAMERA_H
#define CAMERA_H

#include "raylib.h"
#include "raymath.h"
#include <thread>

#define EP 1
#define MAX 100

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

class FoxCamera {
private:
  int width;
  int height;
  int aspectRatio;

  RaycastRay **rays;

public:
  FoxCamera(int cWidth, int cHeight);
  ~FoxCamera();
  void Render(Image *image);
  Vector3 position;
  Vector3 rotation;
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

FoxModel makeCube();
FoxTri **vertToTri(float *vert, int size);

void proccessPixel(Image *image, int x, int y, RaycastRay *ray);
#endif
