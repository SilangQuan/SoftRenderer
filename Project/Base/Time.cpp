#include "Time.h"
#include "SDL2/SDL.h"
#include "Log/Log.h"
float Time::deltaTime = 0;
float Time::time = 0;
float Time::fps = 0;

float inv1000 = 1.0f / 1000;
Time::Time()
{
	frameCountInOneSecond = 0;
}

Time::~Time()
{
	
}

void Time::Start()
{
	startMark = SDL_GetTicks() * inv1000;
	lastFpsMark = startMark;
	lastUpdateMark = SDL_GetTicks()* inv1000;
}

void Time::Stop()
{
	
}

void Time::Restart()
{
	
}

void Time::Update()
{
	lastUpdateMark = time;
	time = SDL_GetTicks() * inv1000 - startMark;
	deltaTime = time - lastUpdateMark;
	frameCountInOneSecond++;
	if (time - lastFpsMark > 1)
	{
		fps = frameCountInOneSecond;
		frameCountInOneSecond = 0;
		lastFpsMark = time;
	}
	//qDebug() <<"deltaTime" << deltaTime;
	//fps = 1.0 / deltaTime;
}