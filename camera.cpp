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

FoxCamera::FoxCamera(int cWidth, int cHeight) {
  width = cWidth;
  height = cHeight;
  vmax = MAX_THREADS;
  aspectRatio = cWidth / cHeight;
  rotation = Vector3Zero();

  rays = (RaycastRay **)malloc(sizeof(RaycastRay *) * width * height);

  for (int y = 0; y < width; y++) {
    for (int x = 0; x < height; x++) {
      float xx = (FOV * ((x + 0.5) * 1 / width) - 1) * aspectRatio;
      float yy = (1 - FOV * ((y + 0.5) * 1 / height));
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
void FoxCamera::RenderChunk(Image *image, int chunk,
                            std::vector<FoxModel> *models) {
  for (int y = 0; y < height; y += vmax) {
    for (int x = 0; x < width; x++) {

      RaycastRay *ray = rays[(x) + ((y + chunk) * width)];
      ray->origin = position;
      ray->rotation = rotation;
      ray->directionComputed =
          Vector3RotateByAxisAngle(ray->direction, {1, 0, 0}, ray->rotation.x);
      ray->directionComputed = Vector3RotateByAxisAngle(
          ray->directionComputed, {0, -1, 0}, ray->rotation.y);

      proccessPixel(image, x, y + chunk, ray, models);
    }
  }

  offset = !offset;
}
void *FoxCamera::ThreadHelper(void *context) {
  ChunkInfo *chunk = static_cast<ChunkInfo *>(context);
  chunk->camera->RenderChunk(chunk->image, chunk->chunk, chunk->models);
  return NULL;
}
void FoxCamera::Render(Image *image, std::vector<FoxModel> *models) {
  for (int threadId = 0; threadId < MAX_THREADS; threadId++) {
    ChunkInfo *chunk = new ChunkInfo{this, image, threadId, models};
    pthread_create(&threads[threadId], NULL, &FoxCamera::ThreadHelper,
                   (void *)chunk);
  }
  for (int threadId = 0; threadId < MAX_THREADS; threadId++) {
    pthread_join(threads[threadId], NULL);
  }
}

void proccessPixel(Image *image, int x, int y, RaycastRay *ray,
                   std::vector<FoxModel> *models) {
  ImageDrawRectangleRec(image,
                        {(float)x * SCALE, (float)y * SCALE, SCALE, SCALE},
                        ray->GetColor(models));
}

RaycastRay::RaycastRay(Vector3 cDirection, Vector3 cOrigin) {
  direction = cDirection;
  origin = cOrigin;
}

RaycastRay::~RaycastRay() {}

Color RaycastRay::GetColor(std::vector<FoxModel> *models) {

  Color color = GRAY;
  float topScale = INFINITY;
  int topHue = 0;
  Vector3 topNormal;

  for (std::vector<FoxModel>::iterator model = models->begin();
       model != models->end(); model++) {
    for (int i = 0; i < model->size; i++) {
      float scalar = model->tris[i]->IntersectsRay(*this, topScale);

      if (scalar == -1 || scalar >= topScale)
        continue;

      topScale = scalar;
      topNormal = model->tris[i]->normal;
      topHue = model->hue;
    }
  }

  if (topScale != INFINITY) {

    color = ColorFromHSV(
        topHue, 100,
        Clamp(255 - std::fabs(Vector3DotProduct(Vector3Normalize(topNormal),
                                                directionComputed)) /
                        300,
              0, 255));
  }
  return color;
}

FoxPlane::FoxPlane(Vector3 cNormal, Vector3 cOrigin) {
  normal = cNormal;
  origin = cOrigin;
}

FoxPlane::~FoxPlane() {}

float FoxPlane::DistanceToPoint(Vector3 point) {
  // Distance = |Ax + By + Cz + D| / sqrt(A^2 + B^2 + C^2)
  // (A, B, C) is the normal vector of the plane.
  //  (x, y, z) are the coordinates of the center of the sphere.
  // D is the distance from the origin to the plane along its normal vector.
  // D = -(A * x0 + B * y0 + C * z0)
  // (x0, y0, z0) are the coordinates of a point on the plane. You can choose
  // any point on the plane to calculate D.
  float distFromOrigin = -Vector3DotProduct(normal, origin);
  float offsetDistanceOrigin = fabs(normal.x * point.x + normal.y * point.y +
                                    normal.z * point.z + distFromOrigin);
  return offsetDistanceOrigin / Vector3Length(normal);
}
Vector3 FoxPlane::PointForDistance(float dist, Vector3 point) {
  Vector3 closestPointVector = Vector3Scale(Vector3Normalize(normal), dist);
  return Vector3Add(closestPointVector, point);
}

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
  return PointInside(Q, scalar);
}
float FoxTri::PointInside(Vector3 point, float scalar) {
  if (Vector3DotProduct(Vector3CrossProduct(BASub, Vector3Subtract(point, A)),
                        normal) >= 0 &&
      Vector3DotProduct(Vector3CrossProduct(CBSub, Vector3Subtract(point, B)),
                        normal) >= 0 &&
      Vector3DotProduct(Vector3CrossProduct(ACSub, Vector3Subtract(point, C)),
                        normal) >= 0) {
    return scalar;
  }
  return -1;
}
// https://cseweb.ucsd.edu/classes/sp19/cse291-d/Files/CSE291_13_CollisionDetection.pdf
float FoxTri::IntersectsSphere(Vector3 point, float radius) {
  float dist = raycastPlane->DistanceToPoint(point);
  if (dist > radius || dist < -radius)
    return 0;
  Vector3 pointOnPlane = raycastPlane->PointForDistance(dist, point);
  if (PointInside(pointOnPlane, fabs(dist)) == -1) {
    return 0;
  }
  return dist;
}
