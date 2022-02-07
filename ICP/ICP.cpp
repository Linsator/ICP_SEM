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
* 3D GL (glew), shaders:
*	
* mouse (both axes), keyboard, fullscreen vs. windowed switching:
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
*/

// forward declarations
void init_all();
void reset_projection();


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

/*
callbacks
*/
void error_callback(int error, const char* description) {};
void fbsize_callback(GLFWwindow* window, int width, int height) {};
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {};
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {};
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {};
