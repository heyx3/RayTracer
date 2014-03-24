#include "SFMLOpenGLWorld.h"

#include "OpenGLIncludes.h"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>


void SFMLOpenGLWorld::InitializeWorld(void)
{
	//Set up GLEW.
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		char * cs = (char*)(glewGetErrorString(res));
		OnInitializeError(std::string(cs));
	}

	GetWindow()->setVerticalSyncEnabled(true);
}

void SFMLOpenGLWorld::OnWindowResized(unsigned int newWidth, unsigned int newHeight)
{
    glViewport(0, 0, newWidth, newHeight);
}