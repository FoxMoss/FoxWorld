#include "camera.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

FoxModel makeCube() {
  int tris = 108;
  FoxModel ret;
  ret.tris = (FoxTri **)malloc(sizeof(FoxTri *) * tris);

  float vertices[108] = {
      // Front
      -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
      -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,
      // Right
      0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f,
      0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f,
      // Back
      0.5f, 0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f,
      -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
      // Left
      -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,
      0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,
      // Top
      -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
      -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
      // Bottom
      -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f,
      0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f};

  ret.tris = vertToTri(vertices, tris);
  ret.size = tris / 9;

  return ret;
}

FoxTri **vertToTri(float *vert, int size) {
  if (size % 3 != 0) {
    std::cout << "Model not Tris";
    return nullptr;
  }
  FoxTri **ret = (FoxTri **)malloc(sizeof(FoxTri *) * size / 3);
  Vector3 A;
  Vector3 B;
  Vector3 C;
  Vector3 temp;
  int triProg = 0;
  int triTotal = 0;
  for (int i = 0; i < size; i++) {
    switch (i % 3) {
    case 0:
      temp.x = vert[i];
      break;
    case 1:
      temp.y = vert[i];
      break;
    case 2:
      temp.z = vert[i];
      switch (triProg) {
      case 0:
        A = temp;
        break;
      case 1:
        B = temp;
        break;
      case 2:
        C = temp;
        ret[triTotal] = new FoxTri(A, B, C);
        ++triTotal;
        triProg = -1;
        break;
      }
      ++triProg;
      break;
    }
  }
  return ret;
}
