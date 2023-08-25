#include "camera.hpp"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

// 3488
FoxModel readTris() {
  FoxModel ret;
  int tris = 31392;

  ret.tris = (FoxTri **)malloc(sizeof(FoxTri *) * tris);

  float vertices[31392] = ;

  ret.tris = vertToTri(vertices, tris, {0, 0, 0}, {1, 1, 1});
  ret.size = tris / 9;

  return ret;
}
