#ifndef VERTEXARRAYOBJECT_H_
#define VERTEXARRAYOBJECT_H_

#include <GL/glew.h>

class VertexArrayObject
{
	private:
		int m_numNames;
		GLuint * m_pArrays;
	public:
		void Generate(GLsizei a_numVAOs, GLuint * a_pArrays);
		void Bind(GLuint a_pArray);
		  
		void Unbind();

};
#endif