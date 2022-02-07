#include <opencv2\opencv.hpp>
#include "texture.h"
GLuint textureInit(const char* cesta, const bool mirror, const bool transparent)
{
	cv::Mat image = cv::imread(cesta, cv::IMREAD_UNCHANGED);
	if (image.empty())
	{
		std::cerr << "no texture: " << cesta << std::endl;
		exit(1);
	}
	GLuint texture = tex_gen(image);
	
	if (mirror) {
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	}

	return texture;
}

GLuint tex_gen(cv::Mat& image)
{
	if (image.empty())
	{
		std::cerr << "empty image?" << std::endl;
		exit(1);
	}
	
	bool transparent = (image.channels() == 4);
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//
	//// Try to use compressed textures
	//
	if (glewIsSupported("GL_ARB_texture_compression"))
	{
		GLint num_compressed_format;

		// get list of supported formats
		glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &num_compressed_format);
		if (num_compressed_format > 0)
		{
			GLint compressed, internalformat, compressed_size;

			std::cout << "COMPRESSION supported, tot. available formats: " << num_compressed_format << std::endl;

			// try to load compressed texture
			glHint(GL_TEXTURE_COMPRESSION_HINT, GL_FASTEST);
			//glHint(GL_TEXTURE_COMPRESSION_HINT, GL_NICEST);

			if (transparent)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, image.cols, image.rows, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.data);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, image.cols, image.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, image.data);

			// Is it now really compressed? Did we succeed?
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressed);
			// if the compression has been successful
			if (compressed == GL_TRUE)
			{
				glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalformat);
				glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressed_size);
				std::cout << "ORIGINAL: " << image.total() * image.elemSize() << " COMPRESSED: " << compressed_size << " INTERNAL FORMAT: " << internalformat << std::endl;
			}
		}
		else
		{
			std::cout << "Wtf?" << std::endl;
		}
	}
	else // compression not supported
	{
		// load uncompressed
		if (transparent)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, image.data);
	}

	// Texture filters - pick one

	// nearest neighbor - ugly & fast 
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// bilinear - nicer & slower
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// MIPMAP filtering + automatic MIPMAP generation - nicest, needs more memory. Notice: MIPMAP is only for image minifying.
	// do NOT use with compressed textures
	//glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now.
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // bilinear magnifying
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //trilinear minifying

	return texture;
}