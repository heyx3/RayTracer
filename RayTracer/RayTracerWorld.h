#pragma once

#include "SFMLOpenGLWorld.h"
#include "ThreeDShapes.h"
#include <SFML\Graphics\Image.hpp>
#include "MovingCamera.h"
#include "Lighting.h"

class RayTracerWorld : public SFMLOpenGLWorld
{
public:

	static Vector2i ScreenSize;

	RayTracerWorld(void) : SFMLOpenGLWorld(ScreenSize.x, ScreenSize.y), cam(Vector3f(-10, -10, -10)), light(1.0f) { pixels = 0; screenTex = 0; screenSpr = 0; }
	~RayTracerWorld(void) { DeleteData(); }

protected:

	virtual void InitializeWorld(void) override;

	virtual void UpdateWorld(float elapsedSeconds) override;
	virtual void RenderWorld(float elapsedSeconds) override;

	virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;

	virtual void OnWorldEnd(void) override { DeleteData(); }

private:

	void DeleteData(void);

	void ApplyAntialias(void);

	//Sets up all the necessary threads for ray tracing and waits until they are done running.
	void SetUpThreads(Vector3f incrementX, Vector3f incrementY, Vector3f topLeftScreen);
	//Traces the given collection of pixel rays through the world and puts their colors into the "pixels" array.
	void TracePixelColumns(int firstCol, int lastCol, Vector3f incrementX, Vector3f incrementY, Vector3f topLeftScreen);
	//Traces a ray through the world to get the color for a single pixel.
	Vector3b TracePixel(Vector3f pixelStart, Vector3f direction, int pixelX, int pixelY) const;

	struct Object { std::shared_ptr<Shape> shape; Vector3b color; Object(std::shared_ptr<Shape> _shape, Vector3b col) : shape(_shape), color(col) { } };
	std::vector<Object> shapes;
    std::vector<bool> isShapeTouching;

	sf::Uint8 * pixels;
	sf::Uint8 * GetPixelStart(int x, int y) { return &pixels[(x * 4) + (y * ScreenSize.x * 4)]; }
	sf::Texture * screenTex;
	sf::Sprite * screenSpr;
	
	sf::Font debugFont;

	MovingCamera cam;
	DirectionalLight light;
	float GetLightValue(Vector3f normal, Vector3f surfacePos, Vector3f cameraPos, bool isCenter) const;

	static const Vector3b screenClearColor;
};