
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include "3rdParty/SDL2/include/SDL.h"
#include "Base/Csdl2.h"
#include "Base/Color.h"
#include "Rasterizer.h"
#include "Math/Random.h"

//#include "SDL_test_common.h"
void handleKeyEvent(SDL_Keycode keyCode)
{
	switch (keyCode)
	{
	case SDLK_ESCAPE:
		SDL_Quit();
		/* Exit program. */
		exit(0);
		break;
	case SDLK_SPACE:
		break;
	case SDLK_F1:
		//toggle_fullscreen();
		break;
	default:
		break;
	}
}

void handleEvents()
{
	// Our SDL event placeholder.  
	SDL_Event event;
	//Grab all the events off the queue.  
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			// Handle key Event  
			handleKeyEvent(event.key.keysym.sym);
			break;
		case SDL_QUIT:
			// Handle quit requests (like Ctrl-c).  
			SDL_Quit();
			/* Exit program. */
			exit(0);
			break;
		}
	}
}


int main(int argc, char* args[])
{
	int widowWidth = 640;
	int windowHeight = 360;

	RenderContext renderContext;
	renderContext.width = widowWidth;
	renderContext.height = windowHeight;
	renderContext.bpp = sizeof(uint32);
	renderContext.backBuffer = new uint32[renderContext.width * renderContext.height];
	renderContext.depthBuffer = new float[renderContext.width * renderContext.height];

	Rasterizer rasterizer(&renderContext);
	Color tmpColor = Color::black;
	CSDL2 *sdl = new CSDL2(widowWidth, windowHeight, "Soft Renderer");

	while (true)
	{
		handleEvents();
		sdl->Clean(&renderContext, Color::white);	
		for (int i = 0; i < 100; i++)
		{
			rasterizer.DrawPixel(widowWidth * Random::Value(), windowHeight * Random::Value(), Color::red);
		}

		sdl->SwapBuffer(&renderContext);
	}
	
	delete[] renderContext.backBuffer;
	delete[] renderContext.depthBuffer;

	getchar();
	return 0;
}

