#ifndef CAMERA_H
#define CAMERA_H

#include "raylib.h"
#include "raymath.h"

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
  float IntersectsRay(RaycastRay ray);
  Vector3 normal;

  Vector3 A;
  Vector3 B;
  Vector3 C;

private:
  FoxPlane *raycastPlane;
};

struct FoxModel {
  FoxTri **tris;
  int size;
};

FoxModel makeCube();
FoxTri **vertToTri(float *vert, int size);

#endif
