#include "Engine.h"
#include <iostream>





Zayn1::Engine::Engine()
{




}

Zayn1::Engine::~Engine()
{




}




void Zayn1::Engine::Run()
{
	Init();
	Update();
}

void Zayn1::Engine::Init()
{


	std::cout << "Init";


}


void Zayn1::Engine::Update()
{



	while (true)
	{
		std::cout << "Update";
	}


}