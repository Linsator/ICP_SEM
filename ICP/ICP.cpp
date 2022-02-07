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
*
*/

// forward declarations





//---------------------------------------------------------------------
// MAIN
//---------------------------------------------------------------------
int main(int argc, char** argv)
{


	exit(EXIT_SUCCESS);
}


/*
callbacks
*/
void error_callback(int error, const char* description) {};
void fbsize_callback(GLFWwindow* window, int width, int height) {};
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {};
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {};
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {};
