#pragma once
#include <iostream>
#include "3rdParty/SDL2/include/SDL.h"
//#include <SDL2/SDL.h> 
class RenderContext;
class Rect;
class Circle;
class Line2d;
class Color;

class CSDL2
{
public:
	int screenWidth;
	int screenHeight;

	CSDL2();
	CSDL2(int width, int height, const char* windowName);
	~CSDL2();
	void Quit(int code);
	void ToggleFullscreen();
	void HandleKeyEvent(SDL_Keysym* keysym);
	void HandleEvents();
	void Sdldie(const char *msg);
	void CheckSDLError(int line = -1);
	void Render();
	void Clean(RenderContext* renderContext, Color* color);
	void SetDrawColor(Color* color);

	void DrawPixel(int posX, int posY);
	void SwapBuffer(RenderContext* renderContext);
	void DrawRect(Rect* rect);
	void DrawCicle(Circle* circle);
	void DrawLine(Line2d* line);

protected:
private:
	
	SDL_Window *mainwindow;
	SDL_Renderer *renderer;
	SDL_Surface *surface;
	SDL_Texture* mainRt;

	Uint32 rmask, gmask, bmask, amask;
	//Whether the window is windowed or not
	bool windowed;
	//Whether the window is fine
	bool windowOK;
};
