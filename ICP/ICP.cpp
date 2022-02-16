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
#include "shaders.h"
#include "mesh.h"
#include "mesh_gen.h"

#include "lua_engine.h"
#include "lua_interface.h"

// First person "shooter" akorát s lukem. Støílení na terèe, které se pohybují,
// vždy po znièení všech se objeví další a tøeba se zrychlí jejich pohyb.
// 
// Transparentnost: okolo hráèe se budou pohybovat prùhledné pøekážky.
// Threads: pozice oblièeje dává offset pozice hráèe na možnost míøení. 

/* TODO:
* realtime 2D raster processing = tracker (can be separate project), threads:
*	Cam face tracking in separate thread
*	set face coords as avatar offset, separate offset to not mess with jump
*
* multiple moving 3D models, at leats one loaded from file:
*	Moving targets, Bow, Arrow, transparent obstacle
*
* textures:
*	find textures
*	update obj loader and mesh to load textures automatically?
*
* lighting model, at least 2 lights (1x ambient + Nx something else):
*	Update shader to use lighning
*	place lights into scene
*
* correct transparency:
*	swithing glEnable(GL_CULL_FACE);
*
* collisions:
*	Physics of arrow (movement and collision)
*	Create/modify Class to encapsulate meshes and their position for collision (and drawing?)
*
** EXTRAS
** height map textured by height, proper player height coords
** particles
*	arrow hits target vs somthing else
*	wow efect when you reach a winning score
** scripting (useful)
** audio
*	cool sound when hit centre
*
* DONE:
* mouse (both axes), keyboard, fullscreen vs. windowed switching:
* 3D GL (glew), shaders:*
*/

// forward declarations
void init_all();
void reset_projection();
void toFullscreen();
void toWindowed();
void app_loop();
void draw_scene();
void physics_step();
void stat_tracking();
void create_mesh();
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

//global variables
shaders shader;
bool stats = false;
mesh mesh_floor;
mesh mesh_target;
mesh mesh_transparent;
mesh mesh_arrow;
unsigned int target_num = 1;
float env_resistance_multplr = 1.0;

// sem nic nedávat!!!

//---------------------------------------------------------------------
// MAIN
//---------------------------------------------------------------------
int main(int argc, char** argv)
{
	init_all(); // init all in init.cpp

	app_loop();


	finalize(EXIT_SUCCESS);
}

void init_all()
{
	shader = init();
	reset_projection();
	create_mesh();


	//since GL 4.3
	glDebugMessageCallback(MessageCallback, 0);
	glEnable(GL_DEBUG_OUTPUT);

	gl_print_info();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_SMOOTH); //antialiasing
	glEnable(GL_LINE_SMOOTH);

	//transparency blending function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// ALL objects are non-transparent 
	glEnable(GL_CULL_FACE);

	// scene contains semi-transparent objects
	//glDisable( GL_CULL_FACE );                    // no polygon removal
}

void create_mesh()
{
	mesh_target = gen_mesh_cube("resources/placeholder.png");
	mesh_transparent = gen_mesh_floor("resources/transparent.png", 1);
	mesh_floor = gen_mesh_floor("resources/placeholder.png", 1000);
	mesh_arrow = gen_mesh_cube("resources/arrow.png");
}

void app_loop()
{
	// Main application loop:
	// Run until exit is requested.
	while (!glfwWindowShouldClose(globals.window))
	{
		// Clear color buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render here 
		{
			/* TODO camera part
			if (new_frame) {
				frame.copyTo(local_frame);
				local_center_relative = center_relative;
				new_frame = false;

				// flip image vertically: screen coordinates and GL world coordinates have opposite Y-axis orientation
				cv::flip(local_frame, local_frame, 0);
				local_center_relative.y = 1.0f + -1.0f * local_center_relative.y;
			}
			{
				// show image using GL, simple method, direct pixel copy
				//glRasterPos2i(0, 0);
				//glDrawPixels(local_frame.cols, local_frame.rows, GL_BGR, GL_UNSIGNED_BYTE, local_frame.data);
			}
			*/
		}

		//draw_scene(local_center_relative);
		physics_step();
		draw_scene();
		// ...

		// Swap front and back buffers 
		// Calls glFlush() inside
		glfwSwapBuffers(globals.window);

		// Check OpenGL errors
		gl_check_error();

		// Poll for and process events
		glfwPollEvents();
	}
}

void physics_step()
{
	double t = glfwGetTime();
	float g = 9.81;

	if (globals.arrow->exists)
	{
		double prev_t = globals.arrow->previous_time;
		float V0 = globals.arrow->speed;

		float fi = glm::acos(glm::dot(glm::normalize(glm::vec3(globals.arrow->direction.x, 0.0, globals.arrow->direction.z)), glm::normalize(globals.arrow->direction)));
		//float fi = glm::acos((glm::length(glm::dot(glm::vec3(globals.arrow->direction.x, 0.0, globals.arrow->direction.z), globals.arrow->direction))) / (glm::length(glm::vec3(globals.arrow->direction.x, 0.0, globals.arrow->direction.z)) * glm::length(globals.arrow->direction)));
		
		if (globals.arrow->direction.y >= 0)
			fi = glm::pi<float>() / 2 - fi;
		else
			fi = glm::pi<float>() / 2 + fi;
		
		
		float theta = glm::acos(glm::dot(glm::normalize(glm::vec3(0.0, 0.0, 1.0)), glm::normalize(glm::vec3(globals.arrow->direction.x, 0.0, globals.arrow->direction.z))));
		
		if (globals.arrow->direction.x < 0)
			theta = -theta;

		float delta_t = t - prev_t;

		float Vx = V0 * glm::sin(fi) * glm::sin(theta);
		float x = Vx * delta_t;

		float Vy = V0 * glm::cos(fi);
		float y = Vy * delta_t - g * glm::pow(delta_t, 2) / 2;

		float Vz = V0 * glm::sin(fi) * glm::cos(theta);
		float z = Vz * delta_t;

		globals.arrow->position += glm::vec3(x, y, z);
		globals.arrow->direction = glm::normalize(glm::vec3(x, y, z));

		globals.arrow->previous_time = t;
		globals.arrow->speed = globals.arrow->speed * env_resistance_multplr;
	}
}

void draw_scene()
{
	if (stats)
	{
		stat_tracking();
	}
	//
	// DRAW
	//
	double speed_multiplier = 4.0;
	float spin_radius = 10.0f;
	double t = speed_multiplier * glfwGetTime();

	// Set the camera to use avatar
	glm::mat4 mv_m = glm::lookAt(globals.avatar->position, globals.avatar->position + globals.avatar->lookAt, glm::vec3(0.0f, 1.0f, 0.0f));


	// Draw all non-transparent -------------------------------------------------------------------------------------------------

	// plane with texture
	{
		// activate shader with textures support
		shader.activate();
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "uMV_m"), 1, GL_FALSE, glm::value_ptr(mv_m));
		// set diffuse material
		glUniform4fv(glGetUniformLocation(shader.ID, "u_diffuse_color"), 1, glm::value_ptr(glm::vec4(1.0f)));
		//set texture unit
		glActiveTexture(GL_TEXTURE0);
		//send unit number to FS
		glUniform1i(glGetUniformLocation(shader.ID, "tex0"), 0);
		// draw object
		mesh_draw(mesh_floor);
	}

	double phase_shift = (2 * glm::pi<float>())/ target_num;
	for(int i = 0; i < target_num; i ++)
	{
		
		auto target = glm::translate(mv_m, glm::vec3( 50.0f, spin_radius + spin_radius * cos(t + phase_shift*i), spin_radius * sin(t + phase_shift*i)));
		//set material 
		glUniform4fv(glGetUniformLocation(shader.ID, "u_diffuse_color"), 1, glm::value_ptr(glm::vec4(1.0f)));
		reset_projection();
		//scale
		target = glm::scale(target, glm::vec3(10.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "uMV_m"), 1, GL_FALSE, glm::value_ptr(target));
		//set texture unit
		glActiveTexture(GL_TEXTURE0);
		//send unit number to FS
		glUniform1i(glGetUniformLocation(shader.ID, "tex0"), 0);
		//draw
		mesh_draw(mesh_target);
	}

	if (globals.arrow->exists)
	{
		auto arrow = glm::translate(mv_m, globals.arrow->position);
		// arrow = glm::rotate(arrow, glm::pi<float>() * (float)t, globals.arrow->direction);
		//set material 
		glUniform4fv(glGetUniformLocation(shader.ID, "u_diffuse_color"), 1, glm::value_ptr(glm::vec4(1.0f)));
		reset_projection();
		//scale
		arrow = glm::scale(arrow, glm::vec3(1.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "uMV_m"), 1, GL_FALSE, glm::value_ptr(arrow));
		//set texture unit
		glActiveTexture(GL_TEXTURE0);
		//send unit number to FS
		glUniform1i(glGetUniformLocation(shader.ID, "tex0"), 0);
		//draw
		mesh_draw(mesh_arrow);
	}


	//semi-transparent object, colour through Phong model -----------------------------------------------------------------------
	glEnable(GL_BLEND);                         // enable blending
	glDisable(GL_CULL_FACE);                    // no polygon removal
	glDepthMask(GL_FALSE);                        // set Z to read-only


	//draw transparent 
	{

		auto target = glm::translate(mv_m, glm::vec3(10.0f, 2.0f, 0));
		target = glm::rotate(target, glm::pi<float>()/2, glm::vec3(0.0f, 0.0f, 1.0f));
		//set material 
		glUniform4fv(glGetUniformLocation(shader.ID, "u_diffuse_color"), 1, glm::value_ptr(glm::vec4(1.0f)));
		reset_projection();
		//scale
		target = glm::scale(target, glm::vec3(10.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "uMV_m"), 1, GL_FALSE, glm::value_ptr(target));
		//set texture unit
		glActiveTexture(GL_TEXTURE0);
		//send unit number to FS
		glUniform1i(glGetUniformLocation(shader.ID, "tex0"), 0);
		//draw
		mesh_draw(mesh_transparent);
	}

	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);




		
}


void stat_tracking()
{
	// Time measurement, FPS count etc.
	static double time_fps_old = 0.0;
	static double time_frame_old = 0.0;
	static int frame_cnt = 0;
	double time_current, time_frame_delta;

	time_current = glfwGetTime();
	time_frame_delta = time_current - time_frame_old;
	time_frame_old = time_current;

	//FPS
	if (time_current - time_fps_old > 1.0)
	{
		time_fps_old = time_current;
		std::cout << "FPS: " << frame_cnt << std::endl;
		frame_cnt = 0;
	}
	frame_cnt++;
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
	if (globals.shader_ready)
	{
		GLint currProgram;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

		// set projection for all shaders
		glUniformMatrix4fv(glGetUniformLocation(currProgram, "uProj_m"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	}

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
	static bool f_screen = false;
	if ((action == GLFW_PRESS) || (action == GLFW_REPEAT))
	{
		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_F:
			if (f_screen)
				toWindowed();
			else
				toFullscreen();
			f_screen = !f_screen;
			break;
		case GLFW_KEY_B:
			arrowDestroy(*(globals.arrow));
			break;
		case GLFW_KEY_W:
			avatarMoveForward(*(globals.avatar));
			break;
		case GLFW_KEY_S:
			avatarMoveBackward(*(globals.avatar));
			break;
		case GLFW_KEY_A:
			avatarMoveLeft(*(globals.avatar));
			break;
		case GLFW_KEY_D:
			avatarMoveRight(*(globals.avatar));
			break;
		case GLFW_KEY_E:
			avatarMoveUp(*(globals.avatar));
			break;
		case GLFW_KEY_SPACE:
			// jump pls
			break;
		case GLFW_KEY_LEFT_CONTROL:
		case GLFW_KEY_Q:
			avatarMoveDown(*(globals.avatar));
			break;
		case GLFW_KEY_KP_ADD:
			globals.avatar->movement_speed += 1;
			break;
		case GLFW_KEY_KP_SUBTRACT:
			if (globals.avatar->movement_speed > 1) {
				globals.avatar->movement_speed -= 1;
			}
			break;
		case GLFW_KEY_F1:
			stats = stats ? false : true; // switch state
			break;
		case GLFW_KEY_UP:
			target_num++;
			break;
		case GLFW_KEY_DOWN:
			if (target_num > 1) {
				target_num--;
			}
			break;
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

		move_h_angle = move_h_angle * (*(globals.avatar)).mouse_sensitivity;
		move_v_angle = move_v_angle * (*(globals.avatar)).mouse_sensitivity;

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

		globals.avatar->lookAt = glm::normalize(front);

		old_x = xpos;
		old_y = ypos;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			//action
			arrowShoot(*(globals.arrow), *(globals.avatar));
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

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "HIGH: ";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "MEDIUM: ";
		break;
	default:
		return;
		break;
	}

	std::cout << "[GL CALLBACK]: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") <<
		"type = 0x" << std::hex << type <<
		", severity = 0x" << std::hex << severity <<
		", message = '" << message << '\'' << std::endl << std::dec;
}
