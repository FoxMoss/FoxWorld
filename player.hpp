#pragma once
#include "camera.hpp"
#include <raylib.h>

#define SPEED 2
#define STEP 0.1

#define FRICTION 0.7
#define DRAG 0.9

#define COLLISION_DEPTH 2
class Player : FoxCamera {
private:
  Image image;
  Texture textureBuffer;
  Texture shotgun;

  RaycastRay *gunRay;

  Vector3 velocity;
  float collisionRadius;
  Vector3 collisionPosition;
  Vector3 nextPos;
  float playerHeight;
  bool grounded;
  std::vector<FoxModel> models;

  int width;
  int height;

public:
  Player(int width, int height);
  void Draw();
  void Reset();
  void Update();
  void CollisonUpdate();
};
