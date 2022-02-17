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
*	Mostly done, just fix positions of lights
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
void check_collision();
void physics_step();
void stat_tracking();
void create_mesh();
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

struct Light {
	// Gets the color of the light from the main function
	glm::vec4 lightColor;
	// Gets the position of the light from the main function
	glm::vec3 lightPos;
	float intenA;
	float intenB;
	float ambient;
};

//global variables
shaders shader;
bool stats = false;
mesh mesh_floor;
mesh mesh_target;
mesh mesh_transparent;
mesh mesh_arrow;

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
		check_collision();
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
	static double prev_t = t;
	glm::vec3 g = {0, -9.81, 0};
	glm::vec3 drag = {1.25, 1.0, 1.25};

	for (int i = 0; i < globals.arrows.size(); i++)
	{
		Arrow* a = globals.arrows[i];
		// compute how long between steps
		float delta_t = t - prev_t;

		// compute new velocity = apply gravity to direction and slowdown
		a->direction += g * delta_t;
		a->direction -= drag * delta_t;

		//compute new position
		a->position += a->direction * delta_t;

		// update lifeTime
		a->lifeTime -= delta_t;

		// if arrow runs out of time remove it from vector of arrows and delete from memory
		if (a->lifeTime < 0)
			arrowDestroy(a, i);
	}
	prev_t = t;
}

void check_collision()
{
	//nothing yet
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

	// Set the camera to use avatar
	glm::mat4 mv_m = glm::lookAt(globals.avatar->position, globals.avatar->position + globals.avatar->lookAt, glm::vec3(0.0f, 1.0f, 0.0f));


	glm::vec3 lightPos = { 20.0f,5.0f,10.0f };
	glm::vec4 lightColor = { 1.0f,1.0f,1.0f, 1.0f }; // blue so it's visible

	Light lights[4];
	// direct
	lights[0] = { glm::vec4(0.5f,0.5f,0.5f,1.0f), glm::vec3(1.0f,1.0f,0.0f), 1.0f,1.0f,0.2f };

	// point
	lights[1] = { glm::vec4(1.0f,0.0f,0.0f,1.0f), glm::vec3(20.0f,40.0f,0.0f), 1.5f,0.7f,0.0f };
	lights[2] = { glm::vec4(0.0f,1.0f,0.0f,1.0f), glm::vec3(20.0f,0.0f,40.0f), 1.5f,0.7f,0.0f };
	lights[3] = { glm::vec4(0.0f,0.0f,1.0f,1.0f), glm::vec3(20.0f,0.0f,-40.0f), 1.5f,0.7f,0.0f };


	shader.activate();

	// Set Light
	{
		// Direct light
		glUniform4fv(glGetUniformLocation(shader.ID, "lights[0].lightColor"), 1, glm::value_ptr(lights[0].lightColor));
		glUniform3fv(glGetUniformLocation(shader.ID, "lights[0].lightPos"), 1, glm::value_ptr(lights[0].lightPos));
		glUniform1f(glGetUniformLocation(shader.ID, "lights[0].intenA"), lights[0].intenA);
		glUniform1f(glGetUniformLocation(shader.ID, "lights[0].intenB"), lights[0].intenB);
		glUniform1f(glGetUniformLocation(shader.ID, "lights[0].ambient"), lights[0].ambient);

		// point light 1
		glUniform4fv(glGetUniformLocation(shader.ID, "lights[1].lightColor"), 1, glm::value_ptr(lights[1].lightColor));
		glUniform3fv(glGetUniformLocation(shader.ID, "lights[1].lightPos"), 1, glm::value_ptr(lights[1].lightPos));
		glUniform1f(glGetUniformLocation(shader.ID, "lights[1].intenA"), lights[1].intenA);
		glUniform1f(glGetUniformLocation(shader.ID, "lights[1].intenB"), lights[1].intenB);
		glUniform1f(glGetUniformLocation(shader.ID, "lights[1].ambient"), lights[1].ambient);
		auto l1 = glm::translate(mv_m, lights[1].lightPos);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "uMV_m"), 1, GL_FALSE, glm::value_ptr(l1));
		mesh_draw(mesh_target);

		// point light 2
		glUniform4fv(glGetUniformLocation(shader.ID, "lights[2].lightColor"), 1, glm::value_ptr(lights[2].lightColor));
		glUniform3fv(glGetUniformLocation(shader.ID, "lights[2].lightPos"), 1, glm::value_ptr(lights[2].lightPos));
		glUniform1f(glGetUniformLocation(shader.ID, "lights[2].intenA"), lights[2].intenA);
		glUniform1f(glGetUniformLocation(shader.ID, "lights[2].intenB"), lights[2].intenB);
		glUniform1f(glGetUniformLocation(shader.ID, "lights[2].ambient"), lights[2].ambient);
		auto l2 = glm::translate(mv_m, lights[2].lightPos);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "uMV_m"), 1, GL_FALSE, glm::value_ptr(l2));
		mesh_draw(mesh_target);

		// point light 3
		glUniform4fv(glGetUniformLocation(shader.ID, "lights[3].lightColor"), 1, glm::value_ptr(lights[3].lightColor));
		glUniform3fv(glGetUniformLocation(shader.ID, "lights[3].lightPos"), 1, glm::value_ptr(lights[3].lightPos));
		glUniform1f(glGetUniformLocation(shader.ID, "lights[3].intenA"), lights[3].intenA);
		glUniform1f(glGetUniformLocation(shader.ID, "lights[3].intenB"), lights[3].intenB);
		glUniform1f(glGetUniformLocation(shader.ID, "lights[3].ambient"), lights[3].ambient);
		auto l3 = glm::translate(mv_m, lights[3].lightPos);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "uMV_m"), 1, GL_FALSE, glm::value_ptr(l3));
		mesh_draw(mesh_target);
		
		// Gets the Texture Units from the main function
		// Diffuse texture
		//uniform sampler2D tex0;
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader.ID, "tex0"), 0);
		// Gets the position of the camera from the main function
		//uniform vec3 camPos;
		glUniform3fv(glGetUniformLocation(shader.ID, "camPos"), 1, glm::value_ptr(globals.avatar->position));

		
	}


	// Draw all non-transparent -------------------------------------------------------------------------------------------------

	// plane with texture
	{
		// activate shader with textures support
		
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

	// draw targets
	double phase_shift = (2 * glm::pi<float>())/ globals.targets.size();
	for(int i = 0; i < globals.targets.size(); i ++)
	{
		Target* tar = globals.targets[i];

		double t = tar->speed * glfwGetTime();		
		tar->position = glm::vec3(50.0f, tar->radius + tar->radius * cos(t + phase_shift * i), tar->radius * sin(t + phase_shift * i));

		auto target = glm::translate(mv_m, tar->position);

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

	for (int i = 0; i < globals.arrows.size(); i++)
	{
		Arrow *a = globals.arrows[i];
		auto arrow = glm::translate(mv_m, a->position);

		// make it bigger
		arrow = glm::scale(arrow, glm::vec3(10.0f));

		//rotation facing right direction
		float theta = glm::acos(glm::dot(glm::normalize(glm::vec3(0.0, 0.0, 1.0)), glm::normalize(glm::vec3(a->direction.x, 0.0, a->direction.z))));
		if (a->direction.x < 0)
			theta = -theta;
		arrow = glm::rotate(arrow, theta, glm::vec3(0.0f, 1.0f, 0.0f));

		//rotation to match physics
		float fi = glm::acos(glm::dot(glm::normalize(glm::vec3(a->direction.x, 0.0, a->direction.z)), glm::normalize(a->direction)));

		if (a->direction.y < 0)
			fi = - fi;
		arrow = glm::rotate(arrow, fi, glm::vec3(-1.0f, 0.0f, 0.0f));
		//
		/*else if (globals.arrow->direction.z < 0)
		{
			fi = fi;
			arrow = glm::rotate(arrow, fi, glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(globals.arrow->direction)));

		}*/
		//set material 
		glUniform4fv(glGetUniformLocation(shader.ID, "u_diffuse_color"), 1, glm::value_ptr(glm::vec4(1.0f)));
		reset_projection();
		
		//scale
		arrow = glm::scale(arrow, glm::vec3(0.2f, 0.2f, 1.0f));
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
	for (int i = 0; i < globals.transparents.size(); i++)
	{
		Transparent* transp = globals.transparents[i];
		transp->position = glm::vec3(8.0f, 2.0f, 0);

		auto target = glm::translate(mv_m, transp->position);
		target = glm::rotate(target, glm::pi<float>()/2, glm::vec3(0.0f, 0.0f, 1.0f));
		//set material 
		//glUniform4fv(glGetUniformLocation(shader.ID, "u_diffuse_color"), 1, glm::value_ptr(glm::vec4(1.0f)));
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
			globals.arrows.clear();
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
		case GLFW_KEY_T:
			transparentAdd();
			break;
		case GLFW_KEY_M:
			targetAdd();
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
			if (globals.avatar->movement_speed > 1) 
			{
				globals.avatar->movement_speed -= 1;
			}
			break;
		case GLFW_KEY_F1:
			stats = stats ? false : true; // switch state
			break;
		case GLFW_KEY_UP:
			targetAdd();
			break;
		case GLFW_KEY_DOWN:
			if (globals.targets.size() > 1) 
			{
				targetDestroy(globals.targets[globals.targets.size() - 1], globals.targets.size() - 1);
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
			arrowShoot();
			// std::cout << "Left mouse button pressed!" << std::endl;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			//action
			// std::cout << "Right mouse button pressed!" << std::endl;
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
