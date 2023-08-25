#include "camera.hpp"
#include "raylib.h"
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <raymath.h>
#include <string>
#include <thread>
#include <vector>

Vector3 A = {1, -1, 1};
Vector3 B = {-1, -1, 1};
Vector3 C = {0, 1, 1};
FoxTri *tri = new FoxTri(A, B, C);

void setupModels() { models.push_back(makeTorus({0, -3, 0}, {1, 1, 1})); }

FoxCamera::FoxCamera(int cWidth, int cHeight) {
  width = cWidth;
  height = cHeight;
  vmax = MAX_THREADS;
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
void FoxCamera::RenderChunk(Image *image, int chunk) {
  for (int y = 0; y < height; y += vmax) {
    for (int x = 0; x < width; x++) {

      RaycastRay *ray = rays[(x) + ((y + chunk) * width)];
      ray->origin = position;
      ray->rotation = rotation;
      ray->directionComputed =
          Vector3RotateByAxisAngle(ray->direction, {0, 1, 0}, ray->rotation.y);

      proccessPixel(image, x, y + chunk, ray);
    }
  }

  offset = !offset;
}
void *FoxCamera::ThreadHelper(void *context) {
  ChunkInfo *chunk = static_cast<ChunkInfo *>(context);
  chunk->camera->RenderChunk(chunk->image, chunk->chunk);
  return NULL;
}
void FoxCamera::Render(Image *image) {
  for (int threadId = 0; threadId < MAX_THREADS; threadId++) {
    ChunkInfo *chunk = new ChunkInfo{this, image, threadId};
    pthread_create(&threads[threadId], NULL, &FoxCamera::ThreadHelper,
                   (void *)chunk);
  }
  for (int threadId = 0; threadId < MAX_THREADS; threadId++) {
    pthread_join(threads[threadId], NULL);
  }
}

void proccessPixel(Image *image, int x, int y, RaycastRay *ray) {
  ImageDrawRectangleRec(image,
                        {(float)x * SCALE, (float)y * SCALE, SCALE, SCALE},
                        ray->GetColor());
}

RaycastRay::RaycastRay(Vector3 cDirection, Vector3 cOrigin) {
  direction = cDirection;
  origin = cOrigin;
}

RaycastRay::~RaycastRay() {}

Color RaycastRay::GetColor() {

  Color topColor = GRAY;
  float topScale = INFINITY;
  Vector3 topNormal;

  for (std::vector<FoxModel>::iterator model = models.begin();
       model != models.end(); model++) {
    for (int i = 0; i < model->size; i++) {
      float scalar = model->tris[i]->IntersectsRay(*this, topScale);

      if (scalar == -1 || scalar >= topScale)
        continue;

      topScale = scalar;
      topNormal = model->tris[i]->normal;
      // topColor = RED;
      // topColor = ColorFromHSV(
      //  0, 0, 255 - Vector3DotProduct(model.tris[i]->normal, {1, -1, 0}) *
      //  10);
    }
  }

  if (topScale != INFINITY) {

    topColor = ColorFromHSV(
        Vector3Scale(directionComputed, topScale).z / 4000, 40,
        (255 -
         Vector3DotProduct(Vector3Normalize(topNormal), {-0.2, -1, 0}) * 10));
    // topColor = RED;
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
