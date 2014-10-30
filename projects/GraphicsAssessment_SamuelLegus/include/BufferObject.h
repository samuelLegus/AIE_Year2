#ifndef BUFFER_H_
#define BUFFER_H_

#include <GL/glew.h>

class BufferObject
{
	private:

		GLuint * m_pBuffers;
		int m_numBuffers;

	public:

		BufferObject(int numBuffers = 1);
		~BufferObject();

		void Generate(int a_numNames);
		void Bind(GLenum a_target, GLuint buffer);
		void BufferData(GLenum a_target, GLsizeiptr a_size, const GLvoid * a_pData, GLenum a_usageHint);
		void Unbind();
};

#endif

