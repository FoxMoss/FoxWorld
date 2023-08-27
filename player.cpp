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
  models.push_back(makeCube({0, -1.4, 21}, {10, 1, 10}));
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

  collisionRadius = 0.3 + STEP * SPEED;
  playerHeight = 2;
  models = genModels();
  position = {0, playerHeight, 0};
  collisionPosition = Vector3Zero();
  velocity = Vector3Zero();
  nextPos = Vector3Zero();
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
    velocity = Vector3Add(velocity, {0, 2, 0});
  }
  Vector2 mouseDelta = GetMouseDelta();
  rotation.y -= mouseDelta.x / 800 * SPEED;
  rotation.x += mouseDelta.y / 800 * SPEED;

  nextPos = Vector3RotateByAxisAngle(nextPos, {0, -1, 0}, rotation.y);
  velocity = Vector3Add(velocity, nextPos);

  velocity = Vector3Add(velocity, {0, -0.2, 0});

  velocity.x *= FRICTION;
  velocity.z *= FRICTION;
  velocity.y *= DRAG;

  // nextPos = Vector3Add(velocity, nextPos);
  position = Vector3Add(position, velocity);
  collisionPosition = Vector3Add(velocity, collisionPosition);
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
        if (dist < collisionRadius) {
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
void Player::Reset() {}
