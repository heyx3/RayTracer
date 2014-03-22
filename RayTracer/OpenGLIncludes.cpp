#include "OpenGLIncludes.h"

namespace Exceptions
{
	int InvalidTextureType = 1;
	int InvalidPrimitiveType = 2;
}

GLenum PrimitiveTypeToGLEnum(PrimitiveTypes t)
{
	switch (t)
	{
		case PrimitiveTypes::Points:
			return GL_POINTS;
		case PrimitiveTypes::Triangles:
			return GL_TRIANGLES;
		case PrimitiveTypes::Lines:
			return GL_LINES;
	}

	throw Exceptions::InvalidPrimitiveType;
}

GLenum TextureTypeToGLEnum(TextureTypes t)
{
	switch (t)
	{
		case TextureTypes::Tex_OneD: return GL_TEXTURE_1D;
		case TextureTypes::Tex_TwoD: return GL_TEXTURE_2D;
		case TextureTypes::Tex_ThreeD: return GL_TEXTURE_3D;
		case TextureTypes::Tex_Cube: return GL_TEXTURE_CUBE_MAP;
	}

	throw Exceptions::InvalidTextureType;
}

/*
VertexIndexData::~VertexIndexData(void)
{ 
	if (createdOwnBuffers) 
	{ 
		glDeleteBuffers(numbBuffers, vbos); 
		glDeleteBuffers(numbBuffers, ibos); 
	} 
	
	DeleteArrIfNotNull(nIndices);
	DeleteArrIfNotNull(nVertices);
	DeleteArrIfNotNull(vbos);
	DeleteArrIfNotNull(ibos);
}
*/