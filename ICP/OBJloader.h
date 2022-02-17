#pragma once
#ifndef OBJloader_H
#define OBJloader_H

#include <vector>
#include <glm/fwd.hpp>
#include "mesh.h"

mesh loadOBJ(const char* modelPath, const char* texPath);

#endif
