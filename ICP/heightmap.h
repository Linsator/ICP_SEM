#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <algorithm>

// OpenCV 
#include <opencv2\opencv.hpp>


#include "mesh.h"

glm::vec2 get_subtex_st(const int x, const int y);
glm::vec2 get_subtex_by_height(float height);
mesh HeightMap(cv::Mat& hmap, unsigned int mesh_step_size, const char* texPath);
