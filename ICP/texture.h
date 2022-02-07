#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <opencv2\opencv.hpp>
#include <GL/glew.h>

GLuint textureInit(const char * cesta, const bool mirror, const bool transparent);
GLuint tex_gen(cv::Mat& image);

#endif
