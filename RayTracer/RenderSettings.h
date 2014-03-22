#pragma once

#include "OpenGLIncludes.h"

//Handles the setting/getting of basic OpenGL render settings, e.x. culling.
class RenderSettings
{
public:

	static void SetUseBlending(bool shouldUse) { if (shouldUse) glEnable(GL_BLEND); else glDisable(GL_BLEND); }

	//Sets whether or not to test polygon depth using the depth buffer.
	static void SetDepthTest(bool shouldUse) { if (shouldUse) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST); }
	//Sets whether or not to write to the depth buffer.
	static void SetUseDepthMask(bool shouldUse) { glDepthMask(shouldUse ? GL_TRUE : GL_FALSE); }

	//If true, front faces are culled. If false, back faces are culled.
	static void SetCullFace(bool cullFront) { glCullFace(cullFront ? GL_FRONT : GL_BACK); }
	//Sets to cull all faces (only points/lines are rendered).
	static void SetCullAllFaces(void) { glCullFace(GL_FRONT_AND_BACK); }

	enum Clearable
	{
		COLOR,
		DEPTH,
		STENCIL,
	};
	static void ClearScreen(Clearable * toClear, int nClearables);


private:

	static GLenum ConvertEnum(Clearable cl) { switch (cl) { case Clearable::STENCIL: return GL_STENCIL_BUFFER_BIT; case Clearable::COLOR : return GL_COLOR_BUFFER_BIT; case Clearable::DEPTH: return GL_DEPTH_BUFFER_BIT; default: return 0; } }

	RenderSettings(void) { }
	~RenderSettings(void) { }
};