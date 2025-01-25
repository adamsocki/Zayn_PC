#pragma once
#include "data_types.h"




struct ZaynTime
{
	real32 startTime;

	real32 systemTime;
	real32 prevSystemTime;

	real32 zaynTime;

	real32 deltaTime;
	real32 totalTime;

	int32 frameCount;
	real32 fpsTimer;
};