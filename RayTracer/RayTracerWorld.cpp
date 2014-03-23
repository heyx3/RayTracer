#include "RayTracerWorld.h"

#include "RenderSettings.h"
#include "TransformObject.h"
#include <thread>

Vector2i RayTracerWorld::ScreenSize = Vector2i(500, 500);
const Vector3b RayTracerWorld::screenClearColor = Vector3b(0, 0, 0);

void RayTracerWorld::DeleteData(void)
{
	DeleteAndSetToNull(screenTex);
	DeleteAndSetToNull(screenSpr);
	DeleteAndSetToNull(pixels);
}

const int numbShapes = 2;
void RayTracerWorld::InitializeWorld(void)
{
	SFMLOpenGLWorld::InitializeWorld();

    currentFPSBufferIndex = 0;
    for (unsigned int i = 0; i < fpsBufferSize; ++i)
        previousFPS[i] = 0;

	//Set up rendering.
	OnWindowResized(GetWindow()->getSize().x, GetWindow()->getSize().y);
	DeleteAndSetToNull(screenTex);
	screenTex = new sf::Texture();
	screenTex->create(GetWindow()->getSize().x, GetWindow()->getSize().y);
	DeleteAndSetToNull(screenSpr);
	screenSpr = new sf::Sprite();

	//Create the shapes to ray-trace.
	shapes.insert(shapes.end(), Object(std::shared_ptr<Shape>(new Cube(Vector3f(), Vector3f(50, 50, 50))), Vector3b(255, 255, 255)));
    shapes.insert(shapes.end(), Object(std::shared_ptr<Shape>(new Cube(Vector3f(50, 50, 50), Vector3f(50, 50, 50))), Vector3b(50, 100, 255)));
    isShapeTouching.insert(isShapeTouching.end(), false);
    isShapeTouching.insert(isShapeTouching.end(), false);

	//Set up the camera.
	Vector3f cPos(-100, -100, 100);
	cam.SetPosition(cPos);
	cam.SetRotation(-cPos, Vector3f(0, 0, 1), false);
	cam.Info.FOV = 3.14159f * 0.25f;
	cam.Window = GetWindow();
	cam.SetMoveSpeed(25.0f);

	//Set up the light.
	light = DirectionalLight(0.5f, 0.05f, Vector3f(1, 1, 1), Vector3f(-2, -3, -1).Normalized());

	//Set up debug font.
	debugFont.loadFromFile("times.ttf");
}

int numbThreads = 7;
int sampleSize = 2;
int centerWeight = 16;
void RayTracerWorld::UpdateWorld(float elapsedSeconds)
{
    previousFPS[currentFPSBufferIndex] = 1.0f / elapsedSeconds;
    currentFPSBufferIndex = (currentFPSBufferIndex + 1) % fpsBufferSize;


    for (int i = 0; i < shapes.size(); ++i)
    {
        isShapeTouching[i] = false;

        for (int j = 0; j < shapes.size(); ++j) if (j != i)
        {
            if (shapes[i].shape->TouchingShape(*shapes[j].shape))
            {
                isShapeTouching[i] = true;
                break;
            }
        }
    }

	if (cam.Update(elapsedSeconds))
	{
		EndWorld();
	}

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::I))
        shapes[0].shape->SetCenter(shapes[0].shape->GetCenter() + Vector3f(1.0f, 0.0f, 0.0f));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::K))
        shapes[0].shape->SetCenter(shapes[0].shape->GetCenter() + Vector3f(-1.0f, 0.0f, 0.0f));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::J))
        shapes[0].shape->SetCenter(shapes[0].shape->GetCenter() + Vector3f(0.0f, -1.0f, 0.0f));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::L))
        shapes[0].shape->SetCenter(shapes[0].shape->GetCenter() + Vector3f(0.0f, 1.0f, 0.0f));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::U))
        shapes[0].shape->SetCenter(shapes[0].shape->GetCenter() + Vector3f(0.0f, 0.0f, 1.0f));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::O))
        shapes[0].shape->SetCenter(shapes[0].shape->GetCenter() + Vector3f(0.0f, 0.0f, -1.0f));


	int * toChange = &centerWeight;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
	{
		*toChange -= 1;
		if (*toChange == 0) *toChange = 1;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
	{
		*toChange += 1;
		if (*toChange > (ScreenSize.x * ScreenSize.y))
		{
			*toChange = ScreenSize.x * ScreenSize.y;
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
	{
		*toChange /= 10;
		if (*toChange == 0) *toChange = 1;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
	{
		*toChange *= 10;
		if (*toChange > (ScreenSize.x * ScreenSize.y))
		{
			*toChange = ScreenSize.x * ScreenSize.y;
		}
	}
}

void RayTracerWorld::OnWindowResized(unsigned int w, unsigned int h)
{
	SFMLOpenGLWorld::OnWindowResized(w, h);

	cam.Info.Width = w;
	cam.Info.Height = h;

	ScreenSize.x = w;
	ScreenSize.y = h;

	delete [] pixels;
	pixels = new sf::Uint8[w * h * 4];

	sf::Uint8 * temp;
	for (int x = 0; x < w; ++x)
	{
		for (int y = 0; y < h; ++y)
		{
			temp = GetPixelStart(x, y);
			temp[0] = screenClearColor.x;
			temp[1] = screenClearColor.y;
			temp[2] = screenClearColor.z;
			temp[3] = 255;
		}
	}

	if (!screenTex)
	{
		screenTex = new sf::Texture();
	}
	screenTex->create(w, h);
	screenTex->update(pixels);
	if (!screenSpr)
	{
		screenSpr = new sf::Sprite();
	}
	screenSpr->setTexture(*screenTex, true);
}

Vector3b RayTracerWorld::TracePixel(Vector3f pixelStart, Vector3f pixelDir, int pixelX, int pixelY) const
{
	//Do a ray trace on all the shapes.
	Shape::RayTraceResult traces[numbShapes];
	for (int i = 0; i < shapes.size(); ++i)
	{
		traces[i] = shapes[i].shape->RayHitCheck(pixelStart, pixelDir);
	}
	


	//Find the ray intersection closest to the pixel.
	//Since we're tracing along a ray,
	//    save processing speed and just use a single axis
	//    to check distance between a point and the pixel.

	int dimensionToUse = 2;
	if (pixelDir.x != 0.0f) dimensionToUse = 0;
	else if (pixelDir.y != 0.0f) dimensionToUse = 1;

	int closestResult = -1;
	float closestDist = 999999.0f, tempDist;

	for (int i = 0; i < numbShapes; ++i) if (traces[i].DidHitTarget)
	{
		//Get the distance from the intersection to the pixel.
		tempDist = BasicMath::Abs(traces[i].HitPos[dimensionToUse] - pixelStart[dimensionToUse]);

		//See if it's the closest intersection found so far.
		if (closestResult == -1 || closestDist > tempDist)
		{
			closestResult = i;
			closestDist = tempDist;
		}
	}



	//Return the correct color.
	if (closestResult == -1)
	{
		return screenClearColor;
	}
	else
	{
		//Use simple lighting.
		float lightScale = GetLightValue(traces[closestResult].ReflectNormal, traces[closestResult].HitPos, cam.GetPosition(), (pixelX == ScreenSize.x / 2) && (pixelY == ScreenSize.y / 2));
		Vector3b col = shapes[closestResult].color;
        if (isShapeTouching[closestResult])
            col = Vector3b(255, 0, 0);
		return Vector3b((unsigned char)(col.x * lightScale),
						(unsigned char)(col.y * lightScale),
						(unsigned char)(col.z * lightScale));
	}
}

void RayTracerWorld::TracePixelColumns(int firstCol, int lastCol, Vector3f incrementX, Vector3f incrementY, Vector3f topLeftScreen)
{
	int x, y;
	if (lastCol >= ScreenSize.x) lastCol = ScreenSize.x - 1;
	sf::Uint8 * pixel;
	Vector3b pixelCol;
	Vector3f pixelPos;
	for (x = firstCol; x <= lastCol; ++x)
	{
		for (y = 0; y < ScreenSize.y; ++y)
		{
			pixelPos = topLeftScreen + (incrementX * x) + (incrementY * y);

			pixel = GetPixelStart(x, y);
			pixelCol = TracePixel(pixelPos, (pixelPos - cam.GetPosition()).Normalized(), x, y);

			memcpy((void*)pixel, (void*)(&pixelCol), sizeof(Vector3b));
			pixel[3] = 255;
			//pixel[0] = pixelCol.x;
			//pixel[1] = pixelCol.y;
			//pixel[2] = pixelCol.z;
			//pixel[3] = 255;
		}
	}
}

void RayTracerWorld::ApplyAntialias(void)
{
    return;
	Vector3i sum;
	int numbSamples;
	int x, y, x2, y2;
	sf::Uint8 * pixel;
	//TODO: Should be doing this operation on a new array and copying the data over!
	for (x = 0; x < ScreenSize.x; ++x)
	{
		for (y = 0; y < ScreenSize.y; ++y)
		{
			sum = Vector3i(0, 0, 0);
			numbSamples = 0;

			for (x2 = -sampleSize; x2 <= sampleSize; ++x2) if (x + x2 >= 0 && x + x2 < ScreenSize.x)
			{
				for (y2 = -sampleSize; y2 <= sampleSize; ++y2) if (y + y2 >= 0 && y + y2 < ScreenSize.y)
				{
					pixel = GetPixelStart(x + x2, y + y2);
					sum.x += pixel[0];
					sum.y += pixel[1];
					sum.z += pixel[2];

					numbSamples += 1;

					//Add some extra weight to the actual center pixel by sampling it multiple times.
					if (x2 == 0 && y2 == 0)
					{
						sum.x += centerWeight * pixel[0];
						sum.y += centerWeight * pixel[1];
						sum.z += centerWeight * pixel[2];

						numbSamples += centerWeight;
					}
				}
			}

			sum /= numbSamples;
			pixel = GetPixelStart(x, y);
			pixel[0] = sum.x;
			pixel[1] = sum.y;
			pixel[2] = sum.z;
		}
	}
}

void RayTracerWorld::SetUpThreads(Vector3f incrementX, Vector3f incrementY, Vector3f topLeftScreen)
{
	int colsPerThread = ScreenSize.x / numbThreads;

	std::thread * threads = new std::thread[numbThreads - 1];
	RayTracerWorld * w = this;

	//This main thread will do work too, so only start [numbThreads - 1] other threads.

	for (int i = 1; i < numbThreads; ++i)
	{
		threads[i-1] = std::thread([w, incrementX, incrementY, topLeftScreen, colsPerThread, i]() { w->TracePixelColumns(colsPerThread * i, (colsPerThread * (i + 1)) - 1, incrementX, incrementY, topLeftScreen); });
		//std::cout << "Column start: " << colsPerThread * i << "\n";
		//std::cout << "Column end: " << ((colsPerThread * (i + 1)) - 1) << "\n\n";
	}
	//This thread should do a bit of the work too!
	TracePixelColumns(0, colsPerThread - 1, incrementX, incrementY, topLeftScreen);
	//char dummy;
	//std::cin >> dummy;

	for (int i = 1; i < numbThreads; ++i)
	{
		threads[i-1].join();
	}

	delete [] threads;
}
void RayTracerWorld::RenderWorld(float elapsedSeconds)
{
	//Transform the "screen" to the correct position in the world.
	//We are going from screen coords to world coords, so use the inverse of the wvp matrix.
	Matrix4f w, v, p, vp, wvp;
	Vector3f topLeftScreen, bottomRightScreen, topRightScreen, bottomLeftScreen, middleScreen;
	w.SetAsIdentity();
	cam.GetViewTransform(v);
	p.SetAsPerspProj(cam.Info);
	vp = Matrix4f::Multiply(p, v);
	wvp = Matrix4f::Multiply(vp, w);
	wvp = wvp.Inverse();
	topLeftScreen = wvp.Apply(Vector3f(-1, -1, 0));
	topRightScreen = wvp.Apply(Vector3f(1, -1, 0));
	bottomLeftScreen = wvp.Apply(Vector3f(-1, 1, 0));
	bottomRightScreen = wvp.Apply(Vector3f(1, 1, 0));
	middleScreen = wvp.Apply(Vector3f());

	//Do the ray tracing.
	Vector2f screenSize(GetWindow()->getSize().x, GetWindow()->getSize().y);
	Vector3f incrementX = topRightScreen - topLeftScreen,
			 incrementY = bottomLeftScreen - topLeftScreen;
	incrementX /= screenSize.x;
	incrementY /= screenSize.y;
	if (numbThreads == 1)
	{
		Vector3f counter,
				 camPos = cam.GetPosition();
		int x, y;
		sf::Uint8 * pixel;
		Vector3b pixelCol;
		for (x = 0; x < screenSize.x; ++x)
		{
			for (y = 0; y < screenSize.y; ++y)
			{
				counter = topLeftScreen + (incrementX * x) + (incrementY * y);

				pixel = GetPixelStart(x, y);
				pixelCol = TracePixel(counter, (counter - camPos).Normalized(), x, y);
				//memcpy((void*)pixel, (void*)(&pixelCol), sizeof(Vector3b));
				pixel[0] = pixelCol.x;
				pixel[1] = pixelCol.y;
				pixel[2] = pixelCol.z;
				pixel[3] = 255;
			}
		}
	}
	else SetUpThreads(incrementX, incrementY, topLeftScreen);
	
	ApplyAntialias();

	//Output the values to a Sprite class for drawing.
	screenTex->update(pixels);
	screenSpr->setTexture(*screenTex, true);

	//Render to the window.
    float avgFPS = 0.0f;
    for (unsigned int i = 0; i < fpsBufferSize; ++i) avgFPS += previousFPS[i];
    avgFPS /= (float)fpsBufferSize;
	RenderSettings::Clearable toClear[2] = { RenderSettings::Clearable::COLOR, RenderSettings::Clearable::DEPTH };
	RenderSettings::ClearScreen(toClear, 2);
	GetWindow()->draw(*screenSpr);
	sf::Text t(sf::String(std::string("Blurriness: ") + std::to_string(centerWeight) + ";   FPS: " + std::to_string(avgFPS)), debugFont, 20);
	t.setColor(sf::Color(128, 128, 128, 255));
	GetWindow()->draw(t);
	GetWindow()->display();
}

float RayTracerWorld::GetLightValue(Vector3f normal, Vector3f surfacePos, Vector3f cameraPos, bool isCenter) const
{
	bool print = isCenter && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::G);

	float dot = normal.Dot(light.Direction);
	
	if (dot <= 0.0f) return BasicMath::Clamp(light.AmbientIntensity);// + specDot);

	Vector3f vertexToEye = (cameraPos - surfacePos).Normalized();
	Vector3f lightReflected = -light.Direction.Reflect(normal).Normalized();
	
	float spec = vertexToEye.Dot(lightReflected);
	float specDot = (spec > 0.0f) ? (powf(spec, 64.0f) * 0.5f) : 0.0f;


	return BasicMath::Clamp(dot + light.AmbientIntensity + specDot);
}