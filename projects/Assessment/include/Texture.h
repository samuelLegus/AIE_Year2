#include "stb_image.h"
#include <GL/glew.h>

struct Texture
{
	unsigned int textureID;
	int width;
	int height;
	int format;

	int LoadFromFile(const char * a_path, GLenum target = GL_TEXTURE_2D)
	{
		unsigned char * data = stbi_load(a_path, &width, &height, &format, STBI_default);

		if (data != nullptr)
			printf("File: %s\nWidth: %i\nHeight: %i\nFormat: %i\n", a_path, width, height, format);
		else
		{
			printf("Could not load data.\n");
			printf("File: %s\nWidth: %i\nHeight: %i\nFormat: %i\n", a_path, width, height, format);
			return -1;
		}

		// Convert format from STBI format to OpenGL formnat
		switch (format)
		{
			case STBI_grey:
			{
				format = GL_LUMINANCE;
				break;
			}
			case STBI_grey_alpha:
			{
				format = GL_LUMINANCE_ALPHA;
				break;
			}
			case STBI_rgb:
			{
				format = GL_RGB;
				break;
			}
			case STBI_rgb_alpha:
			{
				format = GL_RGBA;
				break;
			}
			default:
			{
				printf("WARNING: Texture format not supported!\n");
			}
		};

		if (target == GL_TEXTURE_2D)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(	GL_TEXTURE_2D,
						0,
						format,
						width,
						height,
						0,
						format,
						GL_UNSIGNED_BYTE,
						data);

		// Set filtering mode
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Unbind texture so another function won't accidentally change it
		glBindTexture(GL_TEXTURE_2D, 0);

		delete[] data;
		return textureID;
	}
};