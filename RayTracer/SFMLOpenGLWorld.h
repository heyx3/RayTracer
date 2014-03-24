#pragma once

#include "SFMLWorld.h"

//A world that uses SFML and OpenGL.
class SFMLOpenGLWorld : public SFMLWorld
{
public:

	SFMLOpenGLWorld(int windowWidth, int windowHeight) : SFMLWorld(windowWidth, windowHeight) { }
	~SFMLOpenGLWorld(void) { }

protected:

    virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;

	virtual void InitializeWorld(void) override;
	virtual void OnInitializeError(std::string errorMsg) { fprintf(stderr, "Initialization error: '%s'\n", errorMsg.c_str()); }
};