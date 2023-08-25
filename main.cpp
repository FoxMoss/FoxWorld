#include "camera.hpp"
#include "raylib.h"

#define WIDTH 100
#define HEIGHT 100
#define STEP 0.1

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

static void UpdateDrawFrame(void);

FoxCamera *camera = new FoxCamera(WIDTH, HEIGHT);

Image imageBuffer;
Texture textureBuffer;

int main() {

  const int screenWidth = WIDTH;
  const int screenHeight = HEIGHT;

  SetTraceLogLevel(LOG_WARNING);
  InitWindow(screenWidth * SCALE, screenHeight * SCALE, "[FoxEngine]");

  imageBuffer = GenImageColor(screenWidth * SCALE, screenHeight * SCALE, WHITE);
  textureBuffer = LoadTextureFromImage(imageBuffer);
#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else

  SetTargetFPS(60);
  setupModels();

  while (!WindowShouldClose()) {
    UpdateDrawFrame();
  }
#endif

  CloseWindow();
  return 0;
}

static void UpdateDrawFrame(void) {

  if (IsKeyDown(KEY_W)) {
    camera->position.z += STEP;
  }

  if (IsKeyDown(KEY_S)) {
    camera->position.z -= STEP;
  }
  if (IsKeyDown(KEY_D)) {
    camera->position.x += STEP;
  }

  if (IsKeyDown(KEY_A)) {
    camera->position.x -= STEP;
  }

  if (IsKeyDown(KEY_E)) {
    camera->position.y += STEP;
  }

  if (IsKeyDown(KEY_Q)) {
    camera->position.y -= STEP;
  }
  if (IsKeyDown(KEY_RIGHT)) {
    camera->rotation.y += STEP;
  }

  if (IsKeyDown(KEY_LEFT)) {
    camera->rotation.y -= STEP;
  }

  UpdateTexture(textureBuffer, imageBuffer.data);

  BeginDrawing();

  ClearBackground(RED);

  camera->Render(&imageBuffer);

  DrawTexture(textureBuffer, 0, 0, WHITE);

  DrawFPS(10, 10);

  EndDrawing();
}
