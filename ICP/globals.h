#pragma once

// OpenGL Extension Wrangler
#include <GL/glew.h> 
#include <GL/wglew.h> //WGLEW = Windows GL Extension Wrangler (change for different platform) 

// GLFW toolkit
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// OpenCV 
#include <opencv2\opencv.hpp>

// custom Lua wrapper
#include "lua_engine.h"

typedef struct  Avatar {   // camera (player) info

	glm::vec3 position;

	glm::vec3 lookAt; // look direction, not coors where to look at

	float		radius;

	float		mouse_sensitivity;
	bool		lock_cam_move_angles;
	float		movement_speed;
} Avatar;


struct s_globals {
	GLFWwindow* window;
	int height;
	int width;
	double app_start_time;

	Avatar * camera;

	cv::VideoCapture capture;
	lua_engine lua;
};

extern s_globals globals;

Avatar avatarMoveForward(Avatar& avatar);
Avatar avatarMoveBackward(Avatar& avatar);
Avatar avatarMoveLeft(Avatar& avatar);
Avatar avatarMoveRight(Avatar& avatar);
Avatar avatarMoveUp(Avatar& avatar);
Avatar avatarMoveDown(Avatar& avatar);

Avatar avatarRotate(Avatar& avatar, const float yaw, const float pitch, const float roll);
