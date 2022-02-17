#include "heightmap.h"
#include <glm/glm.hpp>
#include <iostream>
#include <algorithm>

// OpenCV 
#include <opencv2\opencv.hpp>


#include "mesh.h"


//return bottom left ST of subtexture
glm::vec2 get_subtex_st(const int x, const int y)
{
	return glm::vec2(x * 1.0f / 2.0f, y * 1.0f / 2.0f);
}

glm::vec2 get_subtex_by_height(float height)
{
	if (height > 0.5)
		return get_subtex_st(0, 1); //rock
	else if (height > 0.2)
		return get_subtex_st(1, 0); //soil
	else 
		return get_subtex_st(0, 0); //grass
}

mesh HeightMap(cv::Mat& hmap, unsigned int mesh_step_size, const char * texPath)
{
	int i = 0;
	mesh mesh;
	std::vector<vertex> vertices;
	std::vector<GLuint> indices;

	glm::vec3 offset = glm::vec3(hmap.cols/2.0f, 0.0f, hmap.rows/2.0f);

	if (hmap.empty())
		return mesh;

	std::cout << "Note: heightmap size:" << hmap.size << ", channels: " << hmap.channels() << std::endl;

	if (hmap.channels() != 1)
	{
		std::cerr << "WARN: requested 1 channel, got: " << hmap.channels() << std::endl;
	}

	// Create heightmap mesh from TRIANGLES in XZ plane, Y is UP (right hand rule)
	//
	//   3-----2
	//   |    /|
	//   |  /  |
	//   |/    |
	//   0-----1
	//
	//   012,023
	//

	for (unsigned int x_coord = 0; x_coord < (hmap.cols - mesh_step_size); x_coord += mesh_step_size)
	{
		for (unsigned int z_coord = 0; z_coord < (hmap.rows - mesh_step_size); z_coord += mesh_step_size)
		{
			// Get The (X, Y, Z) Value For The Bottom Left Vertex = 0
			glm::vec3 p0 = glm::vec3(x_coord, hmap.at<uchar>(cv::Point(x_coord, z_coord)), z_coord);
			// Get The (X, Y, Z) Value For The Bottom Right Vertex = 1
			glm::vec3 p1 = glm::vec3(x_coord + mesh_step_size, hmap.at<uchar>(cv::Point(x_coord + mesh_step_size, z_coord)), z_coord);
			// Get The (X, Y, Z) Value For The Top Right Vertex = 2
			glm::vec3 p2 = glm::vec3(x_coord + mesh_step_size, hmap.at<uchar>(cv::Point(x_coord + mesh_step_size, z_coord + mesh_step_size)), z_coord + mesh_step_size);
			// Get The (X, Y, Z) Value For The Top Left Vertex = 3
			glm::vec3 p3 = glm::vec3( x_coord, hmap.at<uchar>(cv::Point(x_coord, z_coord + mesh_step_size)), z_coord + mesh_step_size );
			
			// Get max normalized height for tile, set texture accordingly
			// Grayscale image returns 0..256, normalize to 0.0f..1.0f by dividing by 256
			
			float max_h = max(hmap.at<uchar>(cv::Point(x_coord, z_coord)) / 256.0f,
				          max(hmap.at<uchar>(cv::Point(x_coord, z_coord + mesh_step_size)) / 256.0f,
				          max(hmap.at<uchar>(cv::Point(x_coord + mesh_step_size, z_coord + mesh_step_size)) / 256.0f,
					               hmap.at<uchar>(cv::Point(x_coord + mesh_step_size, z_coord)) / 256.0f
							)));
			
			

			// Get texture coords in vertices, bottom left of geometry == bottom left of texture
			glm::vec2 tc0 = get_subtex_by_height(max_h);
			glm::vec2 tc1 = tc0 + glm::vec2(1.0f / 2, 0.0f);			//add offset for bottom right corner
			glm::vec2 tc2 = tc0 + glm::vec2(1.0f / 2, 1.0f / 2);  //add offset for top right corner
			glm::vec2 tc3 = tc0 + glm::vec2(0.0f, 1.0f / 2);       //add offset for bottom leftcorner
			
			//place vertices and ST to mesh
			//   3-----2
			//   |    /|
			//   |  /  |
			//   |/    |
			//   0-----1
			//
			// p0 = 0
			// p1 = 3
			// p2 = 2
			// p3 = 1

			glm::vec3 vecFrom1TO0 = p0 - p3;
			glm::vec3 vecFrom1TO2 = p2 - p3;
			glm::vec3 normal0 = glm::normalize(glm::cross(vecFrom1TO2, vecFrom1TO0));

			vertex v0;
			v0.position = p0 - offset;
			v0.texcoord = tc0; 
			v0.normals = normal0;
			
			vertex v1;
			v1.position = p3 - offset;
			v1.texcoord = tc3;
			v1.normals = normal0;

			vertex v2;
			v2.position = p2 - offset;
			v2.texcoord = tc2;
			v2.normals = normal0;

			//------------------------
			glm::vec3 vecFrom5TO3 = p0 - p1;
			glm::vec3 vecFrom5TO4 = p2 - p1;
			glm::vec3 normal1 = glm::normalize(glm::cross(vecFrom5TO3, vecFrom5TO4));

			vertex v3;
			v3.position = p0 - offset;
			v3.texcoord = tc0;
			v3.normals = normal1;

			vertex v4;
			v4.position = p2 - offset;
			v4.texcoord = tc2;
			v4.normals = normal1;

			vertex v5;
			v5.position = p1 - offset;
			v5.texcoord = tc1;
			v5.normals = normal1;

			
			vertices.push_back(v0);
			vertices.push_back(v1);
			vertices.push_back(v2);

			vertices.push_back(v3);
			vertices.push_back(v4);
			vertices.push_back(v5);
			

			// place indices			
			indices.push_back(i);
			indices.push_back(i+1);
			indices.push_back(i+2);
			indices.push_back(i+3);
			indices.push_back(i+4);
			indices.push_back(i+5);
			i += 6;
		
		}
	}
	
	mesh.primitive_type = GL_TRIANGLES;

	cv::Mat image = cv::imread(texPath, cv::IMREAD_UNCHANGED);
	cv::flip(image, image, 0);
	mesh.init(GL_TRIANGLES, vertices, indices, image);
	
	return mesh;
}
