#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.h"
#include "mesh_gen.h"

mesh gen_mesh_floor(std::string tex_path, int size)
{
	mesh tmpmesh;
	std::vector<vertex> vertices = {
	{glm::vec3(size, 0, size),glm::vec2(0.0f, 0.0f)},
	{glm::vec3(size, 0, -size),glm::vec2(1.0f, 0.0f)},
	{glm::vec3(-size, 0, -size),glm::vec2(1.0f, 1.0f)},
	{glm::vec3(-size, 0, size),glm::vec2(0.0f, 1.0f)}
	};
	std::vector<GLuint> indices = { 0,1,2,0,2,3 };
	
	cv::Mat image = cv::imread(tex_path, cv::IMREAD_UNCHANGED);
	cv::flip(image, image, 0);
	tmpmesh.init(GL_TRIANGLES, vertices, indices, image);

	return tmpmesh;
}


mesh gen_mesh_cube_texID(std::string tex_path, int texID)
{
	mesh tmpmesh;

	cv::Mat image = cv::imread(tex_path, cv::IMREAD_UNCHANGED);
	cv::flip(image, image, 0);

	float step = 1.0f/ 16.0f;
	float leftBotOffsetU = (texID % 16) * step;
	float leftBotOffsetV = (texID / 16) * step;

	std::vector<vertex> vertices = {
	{ glm::vec3(-0.5f, 0.0f, 0.5f),glm::vec2(leftBotOffsetU,leftBotOffsetV)},
	{ glm::vec3(0.5f, 0.0f, 0.5f),glm::vec2(leftBotOffsetU + step,leftBotOffsetV) },
	{ glm::vec3(0.5f, 1.0f, 0.5f),glm::vec2(leftBotOffsetU + step, leftBotOffsetV + step)},
	{ glm::vec3(-0.5f, 1.0f, 0.5f),glm::vec2(leftBotOffsetU, leftBotOffsetV + step)},
	{ glm::vec3(-0.5f, 1.0f, -0.5f),glm::vec2(leftBotOffsetU, leftBotOffsetV)},
	{ glm::vec3(0.5f, 1.0f, -0.5f),glm::vec2(leftBotOffsetU + step, leftBotOffsetV)},
	{ glm::vec3(0.5f, 0.0f, -0.5f),glm::vec2(leftBotOffsetU + step, leftBotOffsetV + step)},
	{ glm::vec3(-0.5f, 0.0f, -0.5f),glm::vec2(leftBotOffsetU, leftBotOffsetV + step)},
	{ glm::vec3(0.5f, 0.0f, 0.5f),glm::vec2(leftBotOffsetU, leftBotOffsetV)},
	{ glm::vec3(0.5f, 0.0f, -0.5f),glm::vec2(leftBotOffsetU + step, leftBotOffsetV)},
	{ glm::vec3(0.5f, 1.0f, -0.5f),glm::vec2(leftBotOffsetU + step, leftBotOffsetV + step)},
	{ glm::vec3(0.5f, 1.0f, 0.5f),glm::vec2(leftBotOffsetU, leftBotOffsetV + step)},
	{ glm::vec3(-0.5f, 0.0f, -0.5f),glm::vec2(leftBotOffsetU, leftBotOffsetV)},
	{ glm::vec3(-0.5f, 0.0f, 0.5f),glm::vec2(leftBotOffsetU + step, leftBotOffsetV)},
	{ glm::vec3(-0.5f, 1.0f, 0.5f),glm::vec2(leftBotOffsetU + step, leftBotOffsetV + step)},
	{ glm::vec3(-0.5f, 1.0f, -0.5f),glm::vec2(leftBotOffsetU, leftBotOffsetV + step)},
	};


	std::vector<GLuint> indices = { 0,1,2, 0,2,3, 4,5,6, 4,6,7, 3,2,5, 3,5,4, 7,6,1, 7,1,0, 8,9,10, 8,10,11, 12,13,14, 12,14,15 };



	tmpmesh.init(GL_TRIANGLES, vertices, indices, image);

	return tmpmesh;
}


mesh gen_mesh_cube(std::string tex_path)
{
	mesh tmpmesh;

	cv::Mat image = cv::imread(tex_path, cv::IMREAD_UNCHANGED);
	cv::flip(image, image, 0);

	std::vector<vertex> vertices = {
	{ glm::vec3(-0.5f, 0.0f, 0.5f),glm::vec2(0.0,0.0)},
	{ glm::vec3(0.5f, 0.0f, 0.5f),glm::vec2(1.0,0.0) },	
	{ glm::vec3(0.5f, 1.0f, 0.5f),glm::vec2(1.0,1.0)},
	{ glm::vec3(-0.5f, 1.0f, 0.5f),glm::vec2(0.0, 1.0)},
	{ glm::vec3(-0.5f, 1.0f, -0.5f),glm::vec2(0.0, 0.0)},
	{ glm::vec3(0.5f, 1.0f, -0.5f),glm::vec2(1.0, 0.0)}, 
	{ glm::vec3(0.5f, 0.0f, -0.5f),glm::vec2(1.0, 1.0)},
	{ glm::vec3(-0.5f, 0.0f, -0.5f),glm::vec2(0.0, 1.0)},
	{ glm::vec3(0.5f, 0.0f, 0.5f),glm::vec2(0.0, 0.0)}, 
	{ glm::vec3(0.5f, 0.0f, -0.5f),glm::vec2(1.0, 0.0)}, 
	{ glm::vec3(0.5f, 1.0f, -0.5f),glm::vec2(1.0, 1.0)},
	{ glm::vec3(0.5f, 1.0f, 0.5f),glm::vec2(0.0, 1.0)},
	{ glm::vec3(-0.5f, 0.0f, -0.5f),glm::vec2(0.0, 0.0)},
	{ glm::vec3(-0.5f, 0.0f, 0.5f),glm::vec2(1.0, 0.0)}, 
	{ glm::vec3(-0.5f, 1.0f, 0.5f),glm::vec2(1.0, 1.0)},
	{ glm::vec3(-0.5f, 1.0f, -0.5f),glm::vec2(0.0, 1.0)},
	};


	std::vector<GLuint> indices = { 0,1,2, 0,2,3, 4,5,6, 4,6,7, 3,2,5, 3,5,4, 7,6,1, 7,1,0, 8,9,10, 8,10,11, 12,13,14, 12,14,15 };


	
	tmpmesh.init(GL_TRIANGLES, vertices, indices, image);

	return tmpmesh;
}

mesh gen_mesh_circle(const float radius, unsigned int num_segments)
{
	mesh tmpmesh;
	float theta;
	vertex tmpvertex;

    if (num_segments < 3)
        num_segments = 3;

	tmpvertex.texcoord = glm::vec2(0.0f);

	for (unsigned int u = 0; u < num_segments; u++)
	{
		theta = 2.0f * glm::pi<float>() * u / num_segments;

		tmpvertex.position.x = radius * cosf(theta);
		tmpvertex.position.y = radius * sinf(theta);
		tmpvertex.position.z = 0.0;

		tmpmesh.vertices.push_back(tmpvertex);
		tmpmesh.indices.push_back(u);
	}

	tmpmesh.primitive_type = GL_LINE_LOOP;
	tmpmesh.textured = false;
	tmpmesh.init();

	return tmpmesh;
}

