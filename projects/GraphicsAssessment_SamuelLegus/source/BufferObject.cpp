#include "BufferObject.h"

BufferObject::BufferObject(int a_numBuffers = 1) : m_numBuffers(a_numBuffers)
{
	m_pBuffers = new GLuint[m_numBuffers];
}

BufferObject::~BufferObject()
{
	glDeleteBuffers(m_numBuffers, m_pBuffers);
	delete[] m_pBuffers;
}

void BufferObject::Generate(int a_numNames)
{
	glGenBuffers(a_numNames, m_pBuffers);
}

void BufferObject::Bind(GLenum a_target, GLuint a_buffer)
{
	/*		
		Valid Target(s) :

		Buffer Binding Target			Purpose

		GL_ARRAY_BUFFER					Vertex attributes
		GL_ATOMIC_COUNTER_BUFFER		Atomic counter storage
		GL_COPY_READ_BUFFER				Buffer copy source
		GL_COPY_WRITE_BUFFER			Buffer copy destination
		GL_DISPATCH_INDIRECT_BUFFER		Indirect compute dispatch commands
		GL_DRAW_INDIRECT_BUFFER			Indirect command arguments
		GL_ELEMENT_ARRAY_BUFFER			Vertex array indices
		GL_PIXEL_PACK_BUFFER			Pixel read target
		GL_PIXEL_UNPACK_BUFFER			Texture data source
		GL_QUERY_BUFFER	Query			result buffer
		GL_SHADER_STORAGE_BUFFER		Read-write storage for shaders
		GL_TEXTURE_BUFFER				Texture data buffer
		GL_TRANSFORM_FEEDBACK_BUFFER	Transform feedback buffer
		GL_UNIFORM_BUFFER				Uniform block storage
	*/

	glBindBuffer(a_target, a_buffer);
}

void BufferObject::BufferData(GLenum a_target, GLsizeiptr a_size, const GLvoid * a_pData, GLenum a_usageHint)
{
	/*
		Valid Target(s) :

		Buffer Binding Target			Purpose

		GL_ARRAY_BUFFER					Vertex attributes
		GL_ATOMIC_COUNTER_BUFFER		Atomic counter storage
		GL_COPY_READ_BUFFER				Buffer copy source
		GL_COPY_WRITE_BUFFER			Buffer copy destination
		GL_DISPATCH_INDIRECT_BUFFER		Indirect compute dispatch commands
		GL_DRAW_INDIRECT_BUFFER			Indirect command arguments
		GL_ELEMENT_ARRAY_BUFFER			Vertex array indices
		GL_PIXEL_PACK_BUFFER			Pixel read target
		GL_PIXEL_UNPACK_BUFFER			Texture data source
		GL_QUERY_BUFFER	Query			result buffer
		GL_SHADER_STORAGE_BUFFER		Read-write storage for shaders
		GL_TEXTURE_BUFFER				Texture data buffer
		GL_TRANSFORM_FEEDBACK_BUFFER	Transform feedback buffer
		GL_UNIFORM_BUFFER				Uniform block storage
	*/

	/* 
		Valid Hints:

		STREAM
		The data store contents will be modified once and used at most a few times.

		STATIC
		The data store contents will be modified once and used many times.

		DYNAMIC
		The data store contents will be modified repeatedly and used many times.
	
	*/

	glBufferData(a_target, a_size, a_pData, a_usageHint);
}