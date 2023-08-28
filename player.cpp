#include "player.hpp"
#include "camera.hpp"
#include <cmath>
#include <cstdio>
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <vector>

std::vector<FoxModel> genModels() {
  std::vector<FoxModel> models;
  models.push_back(makeCube({0, -1.4, 0}, {10, 1, 20}));
  models.push_back(makeCube({0, 0, 10}, {10, 2, 10}));
  models.push_back(makeCube({0, 0, 25}, {10, 2, 10}));
  models.push_back(makeCube({20, 0, 25}, {20, 2, 10}));
  models.push_back(makeCube({40, 0, 25}, {5, 2, 5}));
  models.push_back(makeCube({40, 0, 15}, {5, 2, 5}));
  models.push_back(makeCube({40, 0, 5}, {5, 2, 5}));
  models.push_back(makeCube({30, 0, 5}, {5, 2, 5}));
  models.push_back(makeCube({20, 0, 5}, {5, 2, 5}));
  models.push_back(makeCube({10, 0, 0}, {5, 2, 5}));
  models.push_back(makeCube({20, 0, 15}, {5, 5, 5}));
  models.push_back(makeCubeColor({20, 5, 15}, {1, 5, 1}, 180));
  models.push_back(makeCubeColor({0, 0, 0}, {0, 0, 0}, 180));
  return models;
}
Player::Player(int cWidth, int cHeight) : FoxCamera(cWidth, cWidth) {
  width = cWidth;
  height = cHeight;
  image = GenImageColor(width * SCALE, height * SCALE, WHITE);
  textureBuffer = LoadTextureFromImage(image);

  Image shotgunImage = LoadImage("resources/shotgun.png");
  shotgun = LoadTextureFromImage(shotgunImage);
  UnloadImage(shotgunImage);

  Vector3 rayDirection = {0, 0, 1};
  gunRay = new RaycastRay(rayDirection, position);

  collisionRadius = 0.3 + STEP * SPEED;
  playerHeight = 2;
  models = genModels();
  Reset();
}
void Player::Draw() {
  Render(&image, &models);
  UpdateTexture(textureBuffer, image.data);
  DrawTexture(textureBuffer, 0, 0, WHITE);
  DrawTexture(shotgun, width / 2 * SCALE - shotgun.width / 2,
              height * SCALE - shotgun.height, WHITE);
}
void Player::Update() {

  nextPos = Vector3Zero();
  if (IsKeyDown(KEY_W)) {
    nextPos.z += STEP * SPEED;
  }

  if (IsKeyDown(KEY_S)) {
    nextPos.z -= STEP * SPEED;
  }
  if (IsKeyDown(KEY_D)) {
    nextPos.x += STEP * SPEED;
  }

  if (IsKeyDown(KEY_A)) {
    nextPos.x -= STEP * SPEED;
  }

  if (IsKeyDown(KEY_E)) {
    nextPos.y += STEP * SPEED;
  }

  if (IsKeyDown(KEY_Q)) {
    nextPos.y -= STEP * SPEED;
  }
  if (grounded && IsKeyDown(KEY_SPACE)) {
    velocity = Vector3Add(velocity, {0, 1.4, 0});
  }

  if (IsMouseButtonPressed(0)) {
    gunRay->origin = position;
    gunRay->rotation = rotation;
    gunRay->directionComputed = Vector3RotateByAxisAngle(
        gunRay->direction, {1, 0, 0}, gunRay->rotation.x);
    gunRay->directionComputed = Vector3RotateByAxisAngle(
        gunRay->directionComputed, {0, -1, 0}, gunRay->rotation.y);
    float dist = gunRay->GetDistance(&models);
    if (dist != INFINITY) {
      models.pop_back();
      models.push_back(makeCubeColor(
          Vector3Add(Vector3Scale(gunRay->directionComputed, dist),
                     gunRay->origin),
          {1, 1, 1}, 180));
    }
  }
  Vector2 mouseDelta = GetMouseDelta();
  rotation.y -= mouseDelta.x / 800 * SPEED;
  rotation.x += mouseDelta.y / 800 * SPEED;

  nextPos = Vector3RotateByAxisAngle(nextPos, {0, -1, 0}, rotation.y);
  velocity = Vector3Add(velocity, nextPos);

  velocity = Vector3Add(velocity, {0, -0.1, 0});

  velocity.x *= FRICTION;
  velocity.z *= FRICTION;
  velocity.y *= DRAG;

  // nextPos = Vector3Add(velocity, nextPos);
  position = Vector3Add(position, velocity);
  collisionPosition = Vector3Add(velocity, collisionPosition);
  if (collisionPosition.y < -4) {
    Reset();
  }
  CollisonUpdate();
}
void Player::CollisonUpdate() {
  grounded = false;
  for (float collisionHeight = collisionPosition.y;
       collisionHeight <= position.y;
       collisionHeight += playerHeight / COLLISION_DEPTH) {
    Vector3 testPosition = {collisionPosition.x, collisionHeight,
                            collisionPosition.z};
    for (std::vector<FoxModel>::iterator model = models.begin();
         model != models.end(); model++) {
      for (int i = 0; i < model->size; i++) {
        float dist =
            model->tris[i]->IntersectsSphere(testPosition, collisionRadius);

        if (dist == 0)
          continue;
        if (std::fabs(dist) < collisionRadius) {
          position = Vector3Add(
              position, Vector3Scale(Vector3Normalize(model->tris[i]->normal),
                                     collisionRadius - dist));
          collisionPosition =
              Vector3Add(collisionPosition,
                         Vector3Scale(Vector3Normalize(model->tris[i]->normal),
                                      collisionRadius - dist));
          if (Vector3DotProduct(Vector3Normalize(model->tris[i]->normal),
                                {0, 1, 0}) > 0.7) {
            grounded = true;
            velocity.y = 0;
          }
        }
      }
    }
  }
}
void Player::Reset() {
  position = {0, playerHeight, 0};
  collisionPosition = Vector3Zero();
  velocity = {0.0001, 0.0001, 0.0001};
  nextPos = Vector3Zero();
}
