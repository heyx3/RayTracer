#include "SFMLWorld.h"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

typedef sf::Clock Clock;

void SFMLWorld::RunWorld(void)
{
	window = new sf::RenderWindow(sf::VideoMode(windowWidth, windowHeight), "World window");
	Clock cl = Clock();

	totalElapsedSeconds = 0.0f;
	worldOver = false;
	float elapsed;
	sf::Event windowEvent;

	InitializeWorld();

	cl.restart();

	while (window->isOpen() && !worldOver)
	{
		//Handle window events first.
		while (window->pollEvent(windowEvent))
		{
			if (windowEvent.type == sf::Event::Closed)
			{
				OnCloseWindow();
			}
			else if (windowEvent.type == sf::Event::Resized)
			{
				OnWindowResized(windowEvent.size.width, windowEvent.size.height);
			}
			else
			{
				OnOtherWindowEvent(windowEvent);
			}
		}

		//Get elapsed time.
		elapsed = cl.getElapsedTime().asSeconds();
		totalElapsedSeconds += elapsed;
		cl.restart();

		//Call functions.
		if (elapsed > 0.0f)
		{
			UpdateWorld(elapsed);
			RenderWorld(elapsed);
		}
	}

	OnWorldEnd();
}