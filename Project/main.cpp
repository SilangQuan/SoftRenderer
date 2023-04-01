
//#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <vector>

#include "Base/Csdl2.h"
#include "Base/Color.h"
#include "TestCases.h"
#include "Shape/Line2d.h"
#include "Math/Random.h"
#include "Profiler/SimpleProfiler.h"
#include "Apps/Sponza.hpp"
//#include "Apps/LifeOfTriangle.hpp"
//#include "Apps/ClipInHomogeneous.hpp"



int main(int argc, char* args[])
{
	AppBase* app = new Sponza();
	//AppBase* app = new LifeOfTriangle();
	//AppBase* app = new ClipInHomogeneous();
	
	app->Init();

	bool isRun = true;

	while(isRun)
	{
		PROFILE_BEGIN(FrameTime);
		isRun = app->Run();
		PROFILE_END(FrameTime);
	}

	app->Release();
	SimpleProfiler::printAll();	
	getchar();
	return 0;
}