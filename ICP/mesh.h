#pragma once

#include <vector>
#include <string>

#include <opencv2/core.hpp>
#include <glm/fwd.hpp>
#include <GL/glew.h> 
#include <GL/wglew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// vertex definition

struct vertex {
	glm::vec3 position;
	glm::vec2 texcoord;
	glm::vec3 normals;
};

// classes necessary for mesh construction

class VBO {
public:
	GLuint ID;

	VBO(std::vector<vertex> & vertices) {
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ARRAY_BUFFER, ID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vertex), vertices.data(), GL_STATIC_DRAW);
	};

	void bind(void) { glBindBuffer(GL_ARRAY_BUFFER, ID); }
	void unbind(void) {	glBindBuffer(GL_ARRAY_BUFFER, 0); }
	void clear(void) { glDeleteBuffers(1, &ID); }
};

class EBO
{
public:
	GLuint ID;

	EBO(std::vector<GLuint> indices) {
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	};

	void bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); };
	void unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); };
	void clear() { glDeleteBuffers(1, &ID); };
};

class VAO {
public:
	GLuint ID;
	
	VAO(void):ID(0) {};
	
	void init(void) { glGenVertexArrays(1, &ID); }

	// connect VBO Attribute to VAO
	void connect_attrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, void* offset) {
		VBO.bind();
		glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
		glEnableVertexAttribArray(layout);
		VBO.unbind();
	}
	void bind(void) { glBindVertexArray(ID); }
	void unbind(void) {	glBindVertexArray(0); }
	void clear(void) {	glDeleteVertexArrays(1,&ID); }
};

class mesh {
public:
	std::vector<vertex> vertices;
	std::vector<GLuint> indices;
	VAO mesh_VAO;
	GLuint tex_ID = 0;
	
	GLenum primitive_type = GL_POINTS;
	bool textured = false;

	mesh() = default;

	void init(void)
	{
		mesh_VAO.init();
		mesh_VAO.bind();
		
		VBO mesh_VBO(vertices);
		EBO mesh_EBO(indices);

		mesh_VAO.connect_attrib(mesh_VBO, 0, 3, GL_FLOAT, sizeof(vertex), (void*)(0));
		if (textured)
			mesh_VAO.connect_attrib(mesh_VBO, 1, 2, GL_FLOAT, sizeof(vertex), (void*)(0 + offsetof(vertex, texcoord)));
		mesh_VAO.connect_attrib(mesh_VBO, 2, 3, GL_FLOAT, sizeof(vertex), (void*)(0 + offsetof(vertex, normals)));
		mesh_VAO.unbind();
		mesh_VBO.unbind();
		mesh_EBO.unbind();
	}
	void init(GLenum primitive_type, std::vector<vertex>& vertices, std::vector<GLuint>& indices)
	{
		this->primitive_type = primitive_type;
		this->textured = false;
		this->vertices = vertices;
		this->indices = indices;

		init();
	}
	void init(GLenum primitive_type, std::vector<vertex>& vertices, std::vector<GLuint>& indices, cv::Mat& teximage);

	void clear(void) { 
		vertices.clear();
		indices.clear();
		primitive_type = GL_POINTS;
		if (textured)
		{
			textured = false;
			glDeleteTextures(1, &tex_ID);
			tex_ID = 0;
		}
		mesh_VAO.clear();
	}
};

void mesh_draw(mesh& mesh);
