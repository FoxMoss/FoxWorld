#pragma once
#include "camera.hpp"
#include <raylib.h>

#define SPEED 2
#define STEP 0.7
#define FRICTION 0.4

class Player : FoxCamera {
private:
  Image image;
  Texture textureBuffer;
  Texture shotgun;

  Vector3 velocity;
  float collisionRadius;
  Vector3 collisionPosition;
  Vector3 nextPos;
  float playerHeight;
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
