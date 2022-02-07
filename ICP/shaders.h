#pragma once

#include <string>
#include <GL/glew.h> 

std::string textFileRead(const std::string fn);

class shaders {
public:
	GLuint ID = -1;

	// you can add more constructors for pipeline with GS, TS etc.
	shaders(void) = default;
	shaders(std::string VS_file, std::string FS_file);

	void activate(void);
	void deactivate(void);
	void clear(void);

private:
	std::string getShaderInfoLog(const GLuint obj);
	std::string getProgramInfoLog(const GLuint obj);

	GLuint compile_shader(const std::string source_file, const GLenum type);
	GLuint link_shader(const std::vector<GLuint> shader_ids);
};

