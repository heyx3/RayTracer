#pragma once

#include "OpenGLIncludes.h"
#include "ShaderHandler.h"


//Used for getting/setting texture settings.

enum TextureFilterSituation
{
	MIN,
	MAG,
};
enum TextureFilter
{
	LINEAR,
	NEAREST,
};

enum TextureWrapDimension
{
	S,
	T,
};
enum TextureWrapAction
{
	CLAMP,
	REPEAT,
};



//Handles manipulation of OpenGL textures.
class TextureManipulator
{
public:

	static void SetTextureFilter(BufferObjHandle & texObj, TextureFilterSituation sit, TextureFilter val) { glBindTexture(GL_TEXTURE_2D, texObj); glTexParameteri(GL_TEXTURE_2D, ToGLEnum(sit), ToGLInt(val)); }
	static void SetTextureWrap(BufferObjHandle & texObj, TextureWrapDimension dim, TextureWrapAction act) { glBindTexture(GL_TEXTURE_2D, texObj); glTexParameteri(GL_TEXTURE_2D, ToGLEnum(dim), ToGLInt(act)); }

private:

	TextureManipulator(void) { }
	~TextureManipulator(void) { }

	static GLenum ToGLEnum(TextureFilterSituation tfs) { return (tfs == TextureFilterSituation::MAG) ? GL_TEXTURE_MAG_FILTER : GL_TEXTURE_MIN_FILTER; }
	static GLint ToGLInt(TextureFilter tf) { return (tf == TextureFilter::LINEAR) ? GL_LINEAR : GL_NEAREST; }

	static GLenum ToGLEnum(TextureWrapDimension twd) { return (twd == TextureWrapDimension::S) ? GL_TEXTURE_WRAP_S : GL_TEXTURE_WRAP_T; }
	static GLint ToGLInt(TextureWrapAction twa) { return (twa == TextureWrapAction::CLAMP) ? GL_CLAMP : GL_REPEAT; }
};
