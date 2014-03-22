#pragma once

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

//A basic world using SFML.
class SFMLWorld
{
public:

	SFMLWorld(int windWidth, int windHeight)
		: totalElapsedSeconds(0.0f), window(0), windowWidth(windWidth), windowHeight(windHeight) { window = 0; }
	virtual ~SFMLWorld(void) { if (window) delete window; }

	float GetTotalElapsedSeconds(void) const { return  totalElapsedSeconds; }
	sf::RenderWindow * GetWindow(void) const { return window; }

	void SetWindowTitle(const sf::String & title) { window->setTitle(title); }

	void EndWorld(void) { worldOver = true; }

	//Starts running this world in an endless loop until it's finished.
	void RunWorld(void);

protected:

	template<class SomeClass>
	void DeleteAndSetToNull(SomeClass *& pointer) { if (pointer) delete pointer; pointer = 0; }

	virtual void InitializeWorld(void) { }

	virtual void UpdateWorld(float elapsedSeconds) = 0;
	virtual void RenderWorld(float elapsedSeconds) = 0;

	virtual void OnCloseWindow(void) { worldOver = true; }
	virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) { }
	virtual void OnOtherWindowEvent(sf::Event & windowEvent) { }

	virtual void OnWorldEnd(void) { }

private:

	float totalElapsedSeconds;
	sf::RenderWindow * window;
	int windowWidth, windowHeight;

	bool worldOver;
};