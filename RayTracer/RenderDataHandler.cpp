#include "RenderDataHandler.h"

#include <assert.h>
#include "ShaderHandler.h"

char RenderDataHandler::errorMsg[ERROR_MESSAGE_SIZE];
const int RenderDataHandler::EXCEPTION_ELEMENTS_OUT_OF_RANGE = 1;

bool RenderDataHandler::GetUniformLocation(BufferObjHandle shaderProgram, const Char* name, UniformLocation & out_handle)
{
	out_handle = glGetUniformLocation(shaderProgram, name);

	if (!UniformLocIsValid(out_handle))
	{
		SetErrorMsg((std::string("Shader does not contain '") + name + "' (or it was optimized out in compilation).").c_str());
		return false;
	}

	return true;
}

void RenderDataHandler::SetUniformValue(UniformLocation loc, int elements, float * value)
{
	if (elements < 1 || elements > 4)
	{
		throw EXCEPTION_ELEMENTS_OUT_OF_RANGE;
	}

	switch (elements)
	{
		case 1:
			glUniform1f(loc, value[0]);
			break;
		case 2:
			glUniform2f(loc, value[0], value[1]);
			break;
		case 3:
			glUniform3f(loc, value[0], value[1], value[2]);
			break;
		case 4:
			glUniform4f(loc, value[0], value[1], value[2], value[3]);
			break;

		default: assert(false);
	}
}
void RenderDataHandler::SetUniformValue(UniformLocation loc, int elements, int * value)
{
	if (elements < 1 || elements > 4)
	{
		throw EXCEPTION_ELEMENTS_OUT_OF_RANGE;
	}

	switch (elements)
	{
		case 1:
			glUniform1i(loc, value[0]);
			break;
		case 2:
			glUniform2i(loc, value[0], value[1]);
			break;
		case 3:
			glUniform3i(loc, value[0], value[1], value[2]);
			break;
		case 4:
			glUniform4i(loc, value[0], value[1], value[2], value[3]);
			break;

		default: assert(false);
	}
}
void RenderDataHandler::SetMatrixValue(UniformLocation lc, const Matrix4f & mat)
{
	glUniformMatrix4fv(lc, 1, GL_TRUE, (const GLfloat*)(&mat));
}

void RenderDataHandler::CreateTexture2D(BufferObjHandle & texObjectHandle, sf::Image & img)
{
	glGenTextures(1, &texObjectHandle);
	glBindTexture(GL_TEXTURE_2D, texObjectHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
}
void RenderDataHandler::CreateTexture2D(BufferObjHandle & texObjectHandle)
{
	glGenTextures(1, &texObjectHandle);
	glBindTexture(GL_TEXTURE_2D, texObjectHandle);
}

void RenderDataHandler::SetTexture2DDataFloats(const BufferObjHandle & texObjectHandle, Vector2i texSize, Void* pixelData)
{
	glBindTexture(GL_TEXTURE_2D, texObjectHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, texSize.x, texSize.y, 0, GL_RGB, GL_FLOAT, pixelData);
}
void RenderDataHandler::SetTexture2DDataUBytes(const BufferObjHandle & texObjectHandle, Vector2i texSize, Void* pixelData)
{
	glBindTexture(GL_TEXTURE_2D, texObjectHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texSize.x, texSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, pixelData);
}

void RenderDataHandler::DeleteTexture2D(BufferObjHandle & texObjHandle)
{
	glDeleteTextures(1, &texObjHandle);
}

void RenderDataHandler::CreateFrameBuffer(BufferObjHandle & fbo)
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}
void RenderDataHandler::DeleteFrameBuffer(BufferObjHandle & fbo)
{

}
void RenderDataHandler::BindFrameBuffer(BufferObjHandle fbo)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

RenderDataHandler::FrameBufferStatus RenderDataHandler::GetFramebufferStatus(const BufferObjHandle & fbo)
{
	BindFrameBuffer(fbo);
	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	switch (result)
	{
		case GL_FRAMEBUFFER_COMPLETE: return FrameBufferStatus::EVERYTHING_IS_FINE;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: return FrameBufferStatus::BAD_ATTACHMENT;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: return FrameBufferStatus::DIFFERENT_ATTACHMENT_DIMENSIONS;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return FrameBufferStatus::NO_ATTACHMENTS;
		case GL_FRAMEBUFFER_UNSUPPORTED: return FrameBufferStatus::NOT_SUPPORTED;

		default: return FrameBufferStatus::UNKNOWN;
	}
}

void RenderDataHandler::CreateDepthBuffer(BufferObjHandle & dbo, Vector2i size)
{
	glGenRenderbuffers(1, &dbo);
	glBindRenderbuffer(GL_RENDERBUFFER, dbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
}
void RenderDataHandler::SetDepthBufferSize(const BufferObjHandle & dbo, Vector2i size)
{
	BindDepthBuffer(dbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
}
void RenderDataHandler::DeleteDepthBuffer(BufferObjHandle & dbo)
{

}
void RenderDataHandler::BindDepthBuffer(const BufferObjHandle & dbo)
{
	glBindRenderbuffer(GL_RENDERBUFFER, dbo);
}