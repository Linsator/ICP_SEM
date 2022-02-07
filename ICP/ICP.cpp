// icp.cpp 
// Author: JJ

// C++ 
#include <iostream>
#include <chrono>
#include <stack>
#include <random>

// OpenCV 
#include <opencv2\opencv.hpp>

// OpenGL Extension Wrangler
#include <GL/glew.h> 
#include <GL/wglew.h> //WGLEW = Windows GL Extension Wrangler (change for different platform) 

// GLFW toolkit
#include <GLFW/glfw3.h>

// OpenGL math
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// OpenGL textures
#include <gli/gli.hpp>

//project includes...
#include "globals.h"
#include "init.h"
#include "callbacks.h"
#include "glerror.h" // Check for GL errors

#include "lua_engine.h"
#include "lua_interface.h"

// First person "shooter" akorát s lukem. Støílení na terèe, který se pohybujou, vždy po znièení všech se objeví další a tøeba se zrychlí.
// Transparentnost: okolo hráèe se budou pohybovat prùhledný pøekážky.
// Threads: pozice oblièeje dává offset pozice hráèe na možnost míøení. 

/* TODO:
* realtime 2D raster processing = tracker (can be separate project), threads:
*	
* 3D GL (glew), shaders:*	
*	
* multiple moving 3D models, at leats one loaded from file:
*	
* textures:
*	
* lighting model, at least 2 lights (1x ambient + Nx something else):
*	
* correct transparency:
*	
* collisions:
* 
** EXTRAS
** height map textured by height, proper player height coords
** particles
** scripting (useful)
** audio
* 
* DONE:
* mouse (both axes), keyboard, fullscreen vs. windowed switching:
*/

// forward declarations
void init_all();
void reset_projection();
void toFullscreen();
void toWindowed();


// sem nic nedávat!!!

//---------------------------------------------------------------------
// MAIN
//---------------------------------------------------------------------
int main(int argc, char** argv)
{
	init_all(); // init all in init.cpp
	


	finalize(EXIT_SUCCESS);
}

void init_all()
{
	init();
	reset_projection();
}







void reset_projection()
{
	float ratio = globals.width * 1.0f / globals.height;

	glm::mat4 projectionMatrix = glm::perspective(
		glm::radians(60.0f), // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
		ratio,			     // Aspect Ratio. Depends on the size of your window.
		0.1f,                // Near clipping plane. Keep as big as possible, or you'll get precision issues.
		20000.0f              // Far clipping plane. Keep as little as possible.
	);

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

	// set projection for all shaders
	glUniformMatrix4fv(glGetUniformLocation(currProgram, "uProj_m"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	
}

void toFullscreen() {
	glfwGetWindowPos(globals.window, &globals.posX, &globals.posY);
	glfwGetWindowSize(globals.window, &globals.prevW, &globals.prevH);
	// Switch windowed and fullscreen
	// Get primary monitor
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	// Get resolution of monitor
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	// Switch to full screen
	glfwSetWindowMonitor(globals.window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
}

void toWindowed() {
	// Switch windowed and fullscreen
	// Get primary monitor
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	// Switch back to windowed mode
	glfwSetWindowMonitor(globals.window, nullptr, globals.posX, globals.posY, globals.prevW, globals.prevH, 0);
}

/*
callbacks
*/
void error_callback(int error, const char* description)
{
	std::cerr << "Error: " << description << std::endl;
}

void fbsize_callback(GLFWwindow* window, int width, int height)
{
	// check for limit case (prevent division by 0)
	if (height == 0)
		height = 1;

	globals.width = width;
	globals.height = height;

	glViewport(0, 0, width, height);			// set visible area

	reset_projection();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if ((action == GLFW_PRESS) || (action == GLFW_REPEAT))
	{
		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_F:
			toFullscreen();
			break;
		case GLFW_KEY_G:
			toWindowed();
			break;
		case GLFW_KEY_W:
			avatarMoveForward(*(globals.camera));
			break;
		case GLFW_KEY_S:
			avatarMoveBackward(*(globals.camera));
			break;
		case GLFW_KEY_A:
			avatarMoveLeft(*(globals.camera));
			break;
		case GLFW_KEY_D:
			avatarMoveRight(*(globals.camera));
			break;
		case GLFW_KEY_E:
			avatarMoveUp(*(globals.camera));
			break;
		case GLFW_KEY_LEFT_CONTROL:
		case GLFW_KEY_Q:
			avatarMoveDown(*(globals.camera));
			break;
		case GLFW_KEY_KP_ADD:
			globals.camera->movement_speed += 1;
			break;
		case GLFW_KEY_KP_SUBTRACT:
			if (globals.camera->movement_speed > 1) {
				globals.camera->movement_speed -= 1;
			}
			break;
		/* TODO once drawing is added, this is used to disable printing of stats
		case GLFW_KEY_F1:
			stats = stats ? false : true; // switch state
			break;
		*/
		default:
			break;
		}
	}
}

//---------------------------------------------------------------------
// Mose moved?
//---------------------------------------------------------------------
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	static int first = 1;
	static double old_x;
	static double old_y;
	static float cam_h_angle = 0.0f;
	static float cam_v_angle = 0.0f;
	float move_h_angle;
	float move_v_angle;

	if (first) {
		old_x = xpos;
		old_y = ypos;
		first = 0;
	}
	else {
		move_h_angle = -xpos + old_x;
		move_v_angle = -ypos + old_y;

		move_h_angle = move_h_angle * (*(globals.camera)).mouse_sensitivity;
		move_v_angle = move_v_angle * (*(globals.camera)).mouse_sensitivity;

		cam_h_angle = cam_h_angle - move_h_angle;
		cam_v_angle = cam_v_angle + move_v_angle;

		if (cam_v_angle > 89.0f)
		{
			cam_v_angle = 89.0f;
		}
		if (cam_v_angle < -89.0f)
		{
			cam_v_angle = -89.0f;
		}

		glm::vec3 front;
		front.x = cos(glm::radians(cam_h_angle)) * cos(glm::radians(cam_v_angle));
		front.y = sin(glm::radians(cam_v_angle));
		front.z = sin(glm::radians(cam_h_angle)) * cos(glm::radians(cam_v_angle));

		globals.camera->lookAt = glm::normalize(front);

		old_x = xpos;
		old_y = ypos;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			//action
			std::cout << "Left mouse button pressed!" << std::endl;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			//action
			std::cout << "Right mouse button pressed!" << std::endl;
		}
	}
}
