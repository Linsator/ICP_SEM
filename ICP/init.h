#pragma once

#include "globals.h"
#include "shaders.h"

shaders init(void);

void init_glfw(void);
void init_glew(void);
void gl_print_info(void);
shaders init_shader(void);
void init_avatar();
void init_camera();


void finalize(int code);
