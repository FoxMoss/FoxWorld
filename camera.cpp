#include "camera.hpp"
#include "raylib.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <raymath.h>
#include <string>
#include <thread>

Vector3 A = {1, -1, 1};
Vector3 B = {-1, -1, 1};
Vector3 C = {0, 1, 1};
FoxTri *tri = new FoxTri(A, B, C);

FoxModel model;

FoxCamera::FoxCamera(int cWidth, int cHeight) {
  width = cWidth;
  height = cHeight;
  aspectRatio = cWidth / cHeight;

  rays = (RaycastRay **)malloc(sizeof(RaycastRay *) * width * height);

  for (int y = 0; y < width; y++) {
    for (int x = 0; x < height; x++) {
      float xx = (2 * ((x + 0.5) * 1 / width) - 1) * aspectRatio;
      float yy = (1 - 2 * ((y + 0.5) * 1 / height));
      Vector3 rayDirection = {xx, yy, 1};
      rayDirection = Vector3Normalize(rayDirection);

      RaycastRay *ray = new RaycastRay(rayDirection, position);

      rays[x + y * width] = ray;
    }
  }

  model = makeCube();
}

FoxCamera::~FoxCamera() {
  delete tri;
  for (int y = 0; y < width; y++) {
    for (int x = 0; x < height; x++) {
      delete rays[x + y * width];
    }
  }
  free(rays);
}
bool offset;
void FoxCamera::Render(Image *image) {
  for (int y = 0; y < width; y++) {
    for (int x = 0; x < height; x++) {

      if ((y + offset) % 2 == 0)
        continue;

      RaycastRay *ray = rays[x + y * width];
      ray->origin = position;
      ray->rotation = rotation;
      ray->directionComputed =
          Vector3RotateByAxisAngle(ray->direction, {0, 1, 0}, ray->rotation.y);

      proccessPixel(image, x, y, ray);
    }
  }

  offset = !offset;
}

void proccessPixel(Image *image, int x, int y, RaycastRay *ray) {
  ImageDrawPixel(image, x, y, ray->GetColor());
}

RaycastRay::RaycastRay(Vector3 cDirection, Vector3 cOrigin) {
  direction = cDirection;
  origin = cOrigin;
}

RaycastRay::~RaycastRay() {}

Color RaycastRay::GetColor() {

  Color topColor = BLUE;
  float topScale = INFINITY;
  for (int i = 0; i < model.size; i++) {
    float scalar = model.tris[i]->IntersectsRay(*this, topScale);

    if (scalar == -1)
      continue;

    topScale = scalar;
    topColor = ColorFromHSV(
        0, 0, 255 - Vector3DotProduct(model.tris[i]->normal, {1, -1, 0}) * 10);
  }

  return topColor;
}

FoxPlane::FoxPlane(Vector3 cNormal, Vector3 cOrigin) {
  normal = cNormal;
  origin = cOrigin;
}

FoxPlane::~FoxPlane() {}

float FoxPlane::IntersectsRay(RaycastRay ray) {
  float denom = Vector3DotProduct(normal, ray.directionComputed);
  if (denom == 0) {
    return -1;
  }
  float numer = Vector3DotProduct(Vector3Subtract(origin, ray.origin), normal);
  float scalar = numer / denom;
  return scalar;
}

FoxTri::FoxTri(Vector3 cA, Vector3 cB, Vector3 cC) {
  A = cA;
  B = cB;
  C = cC;

  BASub = Vector3Subtract(B, A);
  CBSub = Vector3Subtract(C, B);
  ACSub = Vector3Subtract(A, C);

  normal = Vector3CrossProduct(Vector3Subtract(B, A), Vector3Subtract(C, A));

  raycastPlane = new FoxPlane(normal, A);
}

FoxTri::~FoxTri() { delete raycastPlane; }

float FoxTri::IntersectsRay(RaycastRay ray, float maxDist) {
  const float scalar = raycastPlane->IntersectsRay(ray);
  if (scalar < 0 || scalar >= maxDist) {
    return -1;
  }

  Vector3 Q =
      Vector3Add(Vector3Scale(ray.directionComputed, scalar), ray.origin);

  if (Vector3DotProduct(Vector3CrossProduct(BASub, Vector3Subtract(Q, A)),
                        normal) >= 0 &&
      Vector3DotProduct(Vector3CrossProduct(CBSub, Vector3Subtract(Q, B)),
                        normal) >= 0 &&
      Vector3DotProduct(Vector3CrossProduct(ACSub, Vector3Subtract(Q, C)),
                        normal) >= 0) {
    return scalar;
  }
  return -1;
}
