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

// First person "shooter" akor�t s lukem. St��len� na ter�e, kter� se pohybujou, v�dy po zni�en� v�ech se objev� dal�� a t�eba se zrychl�.
// Transparentnost: okolo hr��e se budou pohybovat pr�hledn� p�ek�ky.
// Threads: pozice obli�eje d�v� offset pozice hr��e na mo�nost m��en�. 

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
