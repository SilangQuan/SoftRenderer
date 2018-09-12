
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
//#include "Apps/Sponza.hpp"
#include "Apps/LifeOfTriangle.hpp"


int main(int argc, char* args[])
{
	//AppBase* app = new Sponza();
	AppBase* app = new LifeOfTriangle();

	app->Init();

	bool isRun = true;
	while(isRun)
	{
		isRun = app->Run();
	}

	app->Release();

	return 0;
}