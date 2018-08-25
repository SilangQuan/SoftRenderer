#include "Csdl2.h"
#include "TypeDef.h"

CSDL2::CSDL2()
{
	//ctor

}
CSDL2::CSDL2(int width, int height, const char* windowName)
{
	screenWidth = width;
	screenHeight = height;

	// Initialize SDL's Video subsystem
	if (SDL_Init(SDL_INIT_VIDEO) < 0) 
		Sdldie("Unable to initialize SDL"); //Or die on error
											
	// Create our window centered at width*height resolution 
	mainwindow = SDL_CreateWindow(windowName, 100, 100, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
	// Die if creation failed
	if (mainwindow == nullptr) 
	{
		Sdldie("Unable to create window");
	}
	else windowed = true;
	
	renderer = SDL_CreateRenderer(mainwindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		Sdldie("Unable to create renderer");
	}

	mainRt = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);

	surface = SDL_GetWindowSurface(mainwindow);
	//mainRt = SDL_GetRenderTarget(renderer);
/*
	/ * Create a 32-bit surface with the bytes of each pixel in R,G,B,A order,
	as expected by OpenGL for textures * /


	/ * SDL interprets each pixel as a 32-bit number, so our masks must depend
	on the endianness (byte order) of the machine * /
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	surface = SDL_CreateRGBSurface(0, width, height, 32,
		rmask, gmask, bmask, amask);
	if (surface == NULL) {
		SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
		exit(1);
	}

	/ * or using the default masks for the depth: * /
	surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);*/

	CheckSDLError(__LINE__);
}

CSDL2::~CSDL2()
{
	//dtor
}


void CSDL2::Quit(int code)
{
	SDL_Quit();
	exit(code);
}

void CSDL2::Sdldie(const char *msg)
{
	std::cout << ("%s: %s\n", msg, SDL_GetError()) << std::endl;
	SDL_Quit();
	exit(1);
}

void CSDL2::HandleEvents()
{
	// Our SDL event placeholder.
	SDL_Event event;
	//Grab all the events off the queue.
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			// Handle key Event
			HandleKeyEvent(&event.key.keysym);
			break;
		case SDL_QUIT:
			// Handle quit requests (like Ctrl-c).
			Quit(0);
			break;
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				if (mainwindow)
				{
					int tmpX, tmpY;
					SDL_GetWindowSize(mainwindow, &tmpX, &tmpY);
				}
			}
			SDL_GL_SwapWindow(mainwindow);
			break;
		}
	}
}

void CSDL2::Render()
{
	SDL_RenderPresent(renderer);
}

void CSDL2::HandleKeyEvent(SDL_Keysym* keysym)
{
	switch (keysym->sym)
	{
	case SDLK_ESCAPE:
		this->Quit(0);
		break;
	case SDLK_SPACE:
		break;
	case SDLK_F1:
		this->ToggleFullscreen();
		break;
	case SDLK_r:
		std::cout << "Fuck" << std::endl;
		break;

	default:
		break;
	}
}

void CSDL2::ToggleFullscreen()
{
	//If the screen is windowed
	if (windowed == true)
	{
		//Set the screen to fullscreen
		int result = SDL_SetWindowFullscreen(mainwindow, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);

		//If there's an error
		if (result < 0)
		{
			windowOK = false;
			return;
		}
		//Set the window state flag
		windowed = false;
	}
	//If the screen is fullscreen
	else if (windowed == false)
	{
		//Window the screen
		SDL_SetWindowSize(mainwindow, screenWidth, screenHeight);
		//Set the window state flag
		windowed = true;
	}
}

void CSDL2::CheckSDLError(int line)
{
#ifndef NDEBUG
	const char *error = SDL_GetError();
	if (*error != '\0')
	{
		printf("SDL Error: %s\n", error);
		if (line != -1)
			printf(" + line: %i\n", line);
		SDL_ClearError();
	}
#endif
}

void CSDL2::SetDrawColor(Color color)
{
	SDL_SetRenderDrawColor(renderer, color.GetUintR(), color.GetUintG(), color.GetUintB(), color.GetUintA());
}

void CSDL2::DrawPixel(int posX, int posY)
{
	SDL_RenderDrawPoint(renderer, posX, posY);
	
}

void CSDL2::SwapBuffer(RenderContext* renderContext)
{
	void* mPixels;
	int mPitch;

	SDL_LockTexture(mainRt, NULL, &mPixels, &mPitch);

	//Copy loaded/formatted surface pixels
	memcpy(mPixels, renderContext->backBuffer, surface->pitch * surface->h);

	//Unlock texture to update
	SDL_UnlockTexture(mainRt);

/*
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderClear(renderer);*/

	SDL_RenderCopy(renderer, mainRt, NULL, NULL);
	SDL_RenderPresent(renderer);

}

void CSDL2::Clean(RenderContext* renderContext, Color color)
{

	int col = (color.GetUintA() << 24) | (color.GetUintR() << 16) | (color.GetUintG() << 8) | (color.GetUintB() << 0);
	memset(renderContext->backBuffer, col, renderContext->width* renderContext->height * renderContext->bpp);
	//memset(g_context.depthBuffer, 3.402823E+38, g_context.width* g_context.height * sizeof(float));
	std::fill(renderContext->depthBuffer, renderContext->depthBuffer + renderContext->width* renderContext->height, FLT_MAX);

	/*SDL_SetRenderDrawColor(renderer, color.GetUintR(), color.GetUintG(), color.GetUintB(), color.GetUintA());
	if (SDL_RenderClear(renderer) != 0)
	{
		std::cout << "fuck" << std::endl;
		//qDebug() << "SDL_RenderClear error";
	}*/
}