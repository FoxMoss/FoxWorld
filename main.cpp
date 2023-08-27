#include "camera.hpp"
#include "player.hpp"
#include "raylib.h"
#include <raymath.h>

#define WIDTH 100
#define HEIGHT 100

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

static void UpdateDrawFrame(void);

Player *player;

Image imageBuffer;
Texture textureBuffer;

int main() {

  const int screenWidth = WIDTH;
  const int screenHeight = HEIGHT;

  SetTraceLogLevel(LOG_WARNING);
  InitWindow(screenWidth * SCALE, screenHeight * SCALE, "[FoxEngine]");
  DisableCursor();

  player = new Player(WIDTH, HEIGHT);

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else

  SetTargetFPS(60 / SPEED);

  while (!WindowShouldClose()) {
    UpdateDrawFrame();
  }
#endif

  CloseWindow();
  return 0;
}

static void UpdateDrawFrame(void) {
  player->Update();

  player->Draw();

  BeginDrawing();

  ClearBackground(RED);

  DrawFPS(10, 10);

  EndDrawing();
}
