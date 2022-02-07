#include <gli/gli.hpp>

#include "init.h"
#include "callbacks.h"
#include "shaders.h"

void init(void)
{
	init_glfw();
	init_glew();
	gl_print_info();
	init_shader();
	init_avatar();
	
	// i don't have camera
	//init_camera();

}

static void init_camera(void)
{
	globals.capture = cv::VideoCapture(cv::CAP_DSHOW);

	if (!globals.capture.isOpened())
	{
		std::cerr << "no camera" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "Camera " <<
			": width=" << globals.capture.get(cv::CAP_PROP_FRAME_WIDTH) <<
			", height=" << globals.capture.get(cv::CAP_PROP_FRAME_HEIGHT) <<
			", FPS=" << globals.capture.get(cv::CAP_PROP_FPS) << std::endl;
	}

	if (!globals.capture.set(cv::CAP_PROP_FRAME_WIDTH, 640))
		std::cout << "Failed width." << std::endl;
	if (!globals.capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480))
		std::cout << "Failed height." << std::endl;
	if (!globals.capture.set(cv::CAP_PROP_FPS, 30))
		std::cout << "Failed FPS." << std::endl;

	std::cout << "Camera changed:" <<
		": width=" << globals.capture.get(cv::CAP_PROP_FRAME_WIDTH) <<
		", height=" << globals.capture.get(cv::CAP_PROP_FRAME_HEIGHT) <<
		", FPS=" << globals.capture.get(cv::CAP_PROP_FPS) << std::endl;
}


void init_avatar() {
	globals.camera = new Avatar();

	globals.camera->position = glm::vec3(0, 10.0f, 0);
	globals.camera->lookAt = glm::vec3(1.0f, 0.0f, 0.0f);
	globals.camera->movement_speed = 1.0f;
	globals.camera->mouse_sensitivity = 0.1f;

	//disable mouse
	glfwSetInputMode(globals.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void init_shader()
{
	//shaders basic_shader = shaders("resources/shaders/basic.vert", "resources/shaders/basic.frag");
	shaders basic_tex_shader = shaders("resources/shaders/basic_tex.vert", "resources/shaders/basic_tex.frag");
	basic_tex_shader.activate();
	globals.shader_ready = true;
}

void init_glfw(void)
{
	//
	// GLFW init.
	//

	// set error callback first
	glfwSetErrorCallback(error_callback);

	//initialize GLFW library
	int glfw_ret = glfwInit();
	if (!glfw_ret)
	{
		std::cerr << "GLFW init failed." << std::endl;
		finalize(EXIT_FAILURE);
	}

	// Shader based, modern OpenGL (3.3 and higher)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // only new functions
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); // only old functions (for old tutorials etc.)

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	globals.window = glfwCreateWindow(800, 600, "OpenGL context", NULL, NULL);
	if (!globals.window)
	{
		std::cerr << "GLFW window creation error." << std::endl;
		finalize(EXIT_FAILURE);
	}

	// Get some GLFW info.
	{
		int major, minor, revision;

		glfwGetVersion(&major, &minor, &revision);
		std::cout << "Running GLFW " << major << '.' << minor << '.' << revision << std::endl;
		std::cout << "Compiled against GLFW " << GLFW_VERSION_MAJOR << '.' << GLFW_VERSION_MINOR << '.' << GLFW_VERSION_REVISION << std::endl;
	}

	glfwMakeContextCurrent(globals.window);										// Set current window.
	glfwGetFramebufferSize(globals.window, &globals.width, &globals.height);	// Get window size.
	glfwSwapInterval(0);														// Set V-Sync OFF.
	//glfwSwapInterval(1);														// Set V-Sync ON.

	//register callbacks
	glfwSetKeyCallback(globals.window, key_callback);
	glfwSetFramebufferSizeCallback(globals.window, fbsize_callback);			// On window resize callback.
	glfwSetMouseButtonCallback(globals.window, mouse_button_callback);
	glfwSetCursorPosCallback(globals.window, cursor_position_callback);
	fbsize_callback(globals.window, globals.width, globals.height);				//initial GL window size setting - call callback manually

	globals.app_start_time = glfwGetTime();										// Get start time.
}

void init_glew(void)
{
	//
	// Initialize all valid GL extensions with GLEW.
	// Usable AFTER creating GL context!
	//
	{
		GLenum glew_ret;
		glew_ret = glewInit();
		if (glew_ret != GLEW_OK)
		{
			std::cerr << "WGLEW failed with error: " << glewGetErrorString(glew_ret) << std::endl;
			finalize(EXIT_FAILURE);
		}
		else
		{
			std::cout << "GLEW successfully initialized to version: " << glewGetString(GLEW_VERSION) << std::endl;
		}

		// Platform specific. (Change to GLXEW or ELGEW if necessary.)
		glew_ret = wglewInit();
		if (glew_ret != GLEW_OK)
		{
			std::cerr << "WGLEW failed with error: " << glewGetErrorString(glew_ret) << std::endl;
			finalize(EXIT_FAILURE);
		}
		else
		{
			std::cout << "WGLEW successfully initialized platform specific functions." << std::endl;
		}
	}
}

void gl_print_info(void)
{
	// Get OpenGL driver info
	{
		const char* vendor_s = (const char*)glGetString(GL_VENDOR);
		const char* renderer_s = (const char*)glGetString(GL_RENDERER);
		const char* version_s = (const char*)glGetString(GL_VERSION);
		const char* glsl_s = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

		std::cout << "OpenGL driver vendor: " << (vendor_s == nullptr ? "<UNKNOWN>" : vendor_s) << ", renderer: " << (renderer_s == nullptr ? "<UNKNOWN>" : renderer_s) << ", version: " << (version_s == nullptr ? "<UNKNOWN>" : version_s) << std::endl;

		GLint profile_flags;
		glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile_flags);
		std::cout << "Current profile: ";
		if (profile_flags & GL_CONTEXT_CORE_PROFILE_BIT)
			std::cout << "CORE";
		else
			std::cout << "COMPATIBILITY";
		std::cout << std::endl;

		GLint context_flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
		std::cout << "Active context flags: ";
		if (context_flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT)
			std::cout << "GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT ";
		if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT)
			std::cout << "GL_CONTEXT_FLAG_DEBUG_BIT ";
		if (context_flags & GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT)
			std::cout << "GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT ";
		if (context_flags & GL_CONTEXT_FLAG_NO_ERROR_BIT)
			std::cout << "GL_CONTEXT_FLAG_NO_ERROR_BIT";
		std::cout << std::endl;

		std::cout << "Primary GLSL shading language version: " << (glsl_s == nullptr ? "<UNKNOWN>" : glsl_s) << std::endl;
	}

	//
	// GLM & GLI library
	//

	std::cout << "GLM version: " << GLM_VERSION_MAJOR << '.' << GLM_VERSION_MINOR << '.' << GLM_VERSION_PATCH << "rev" << GLM_VERSION_REVISION << std::endl;
	std::cout << "GLI version: " << GLI_VERSION_MAJOR << '.' << GLI_VERSION_MINOR << '.' << GLI_VERSION_PATCH << "rev" << GLI_VERSION_REVISION << std::endl;
}

void finalize(int code)
{
	cv::destroyAllWindows();

	// Close OpenCV capture device
	if (globals.capture.isOpened())
		globals.capture.release();

	// Close OpenGL window if opened and terminate GLFW  
	if (globals.window)
		glfwDestroyWindow(globals.window);
	glfwTerminate();

	exit(code);
}