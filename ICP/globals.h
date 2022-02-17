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

typedef struct Target {
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 scale;

	float radius;
	float speed;
} Target;

typedef struct Arrow {
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 scale;

	glm::vec3 bBox_shift;
	glm::vec3 bBox_scale;

	float lifeTime;
	bool canMove;
} Arrow;

typedef struct Transparent {
	glm::vec3 position;
	glm::vec3 scale;

	uint life;
} Transparent;

typedef struct Particle {
	glm::vec3 position;
	glm::vec3 speed;
	glm::vec3 scale;

	float lifeTime;
} Particle;


struct s_globals {
	GLFWwindow* window;
	int height;
	int width;

	// fullscreen/windowed
	int posX;
	int posY;
	int prevH;
	int prevW;

	double app_start_time;

	bool shader_ready = false;

	Avatar * avatar;

	std::vector<Target*> targets;

	std::vector<Arrow*> arrows;

	std::vector<Transparent*> transparents;

	std::vector<Particle*> particles;

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

void arrowShoot();
void arrowDestroy(Arrow* arrow, int at);

void targetAdd();
void targetDestroy(Target* target, int at);

void transparentAdd();
void transparentDestroy(Transparent* transparent, int at);

void particleAdd(Arrow* arrow);