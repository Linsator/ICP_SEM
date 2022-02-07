#pragma once

#include "mesh.h"

mesh gen_mesh_floor(std::string tex_path, unsigned int size);
mesh gen_mesh_circle(const float radius, unsigned int num_segments);
mesh gen_mesh_cube(std::string tex_path);
mesh gen_mesh_cube_texID(std::string tex_path, int texID);
