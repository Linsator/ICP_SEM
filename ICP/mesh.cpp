// mesh.cpp 
// Author: JJ
#include <GL/glew.h> 
#include <GL/wglew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.h"
#include "texture.h"

void mesh::init(GLenum primitive_type, std::vector<vertex>& vertices, std::vector<GLuint>& indices, cv::Mat& teximage)
{
	this->primitive_type = primitive_type;
	this->textured = true;
	this->vertices = vertices;
	this->indices = indices;
	this->tex_ID = tex_gen(teximage);

	init();
}

void mesh_draw(mesh& mesh)
{
	if (mesh.textured)
		glBindTexture(GL_TEXTURE_2D, mesh.tex_ID);

	mesh.mesh_VAO.bind();
	glDrawElements(mesh.primitive_type, mesh.indices.size(), GL_UNSIGNED_INT, 0);
	mesh.mesh_VAO.unbind();

}
