#include <tools/texture_loader>

#include <stdlib.h>

#include <debug>

namespace TextureLoader
{

static GLuint loadDDS(const string &texture_path);
static GLuint loadBMP(const string &texture_path);

GLuint loadTexture(const string &texture_path) {
	if (texture_path.compare(texture_path.size() - 3, 3, "DDS") == 0 || texture_path.compare(texture_path.size() - 3, 3, "dds") == 0) {
		return loadDDS(texture_path);
	}
	if (texture_path.compare(texture_path.size() - 3, 3, "BMP") == 0 || texture_path.compare(texture_path.size() - 3, 3, "bmp") == 0) {
		return loadBMP(texture_path);
	}
}

GLuint loadDDS(const string& texture_path) {
	static const unsigned int FOURCC_DXT1{0x31545844}; // Equivalent to "DXT1" in ASCII
	static const unsigned int FOURCC_DXT3{0x33545844}; // Equivalent to "DXT3" in ASCII
	static const unsigned int FOURCC_DXT5{0x35545844}; // Equivalent to "DXT5" in ASCII

	/* try to open the file */
	FILE *fp = fopen(texture_path.c_str(), "rb");
	if (fp == NULL) {
		fprintf(stderr, "%s could not be opened.\n", texture_path.c_str());
		throw "Load DDS: can not open file error";
	}

	/* verify the type of file */
	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0) {
		fclose(fp);
		fprintf(stderr, "Not a DDS file.\n");

		throw "Load DDS: not DDS file error";
	}

	/* get the surface desc */
	unsigned char header[124];
	fread(&header, 124, 1, fp);

	unsigned int height      	= *(unsigned int*) & (header[8 ]);
	unsigned int width	     	= *(unsigned int*) & (header[12]);
	unsigned int linear_size 	= *(unsigned int*) & (header[16]);
	unsigned int mip_map_count 	= *(unsigned int*) & (header[24]);
	unsigned int four_cc      	= *(unsigned int*) & (header[80]);

	unsigned int bufsize = mip_map_count > 1 ? linear_size * 2 : linear_size;
	unsigned char *buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
	fread(buffer, 1, bufsize, fp);
	fclose(fp);

	unsigned int components  = (four_cc == FOURCC_DXT1) ? 3 : 4;
	unsigned int format;
	switch (four_cc) {
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		free(buffer);
		fprintf(stderr, "Format not known\n");
		throw "Load DDS: format not known error";
	}

	// Create one OpenGL texture
	GLuint texture_ID;
	glGenTextures(1, &texture_ID);

	// "Bind" the texture
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned int block_size = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	/* load the mipmaps */
	for (unsigned int level = 0; level < mip_map_count && (width || height); ++level) {
		unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * block_size;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer + offset);

		offset += size;
		width  /= 2;
		height /= 2;

		// Deal with Non-Power-Of-Two textures.
		if (width < 1) {
			width = 1;
		}
		if (height < 1) {
			height = 1;
		}
	}
	free(buffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	return texture_ID;
}

GLuint loadBMP(const string& texture_path) {
	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int data_pos;
	unsigned int image_size;
	unsigned int width, height;
	// Actual RGB data
	unsigned char * data;

	// Open the file
	FILE * file = fopen(texture_path.c_str(), "rb");
	if (!file) {
		fprintf(stderr, "%s could not be opened.\n", texture_path.c_str());
		throw "Load BMP: can not open file error";
	}

	// Read the header, i.e. the 54 first bytes
	// If less than 54 bytes are read, problem
	if ( fread(header, 1, 54, file) != 54 ) {
		fprintf(stderr, "Not a correct BMP file\n");
		throw "Load BMP: not BMP file error";
	}
	// A BMP files always begins with "BM"
	if ( header[0] != 'B' || header[1] != 'M' ) {
		fprintf(stderr, "Not a correct BMP file\n");
		throw "Load BMP: not BMP file error";
	}
	// Make sure this is a 24bpp file
	if ( *(int*) & (header[0x1E]) != 0  ) {
		fprintf(stderr, "Not a correct BMP file\n");
		throw "Load BMP: not BMP file error";
	}
	if ( *(int*) & (header[0x1C]) != 24 ) {
		fprintf(stderr, "Not a correct BMP file\n");
		throw "Load BMP: not BMP file error";
	}

	// Read the information about the image
	data_pos    = *(int*) & (header[0x0A]);
	image_size  = *(int*) & (header[0x22]);
	width      = *(int*) & (header[0x12]);
	height     = *(int*) & (header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (image_size == 0)    image_size = width * height * 3; // 3 : one byte for each Red, Green and Blue component
	if (data_pos == 0)      data_pos = 54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char[image_size];

	// Read the actual data from the file into the buffer
	fread(data, 1, image_size, file);

	// Everything is in memory now, the file wan be closed
	fclose (file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	// OpenGL has now copied the data. Free our own version
	delete [] data;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Return the ID of the texture we just created
	return textureID;
}

}