//zayn.cpp
#include "zayn.h"




void ZaynInit(ZaynMemory* zaynMem)
{
	std::cout << "zaynInit()" << std::endl;

	InitRender(zaynMem);
}







void ZaynUpdateAndRender(ZaynMemory* zaynMem)
{
	UpdateRender(zaynMem);

}
