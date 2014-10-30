#include "AdvancedTextureTutorial.h"
#include "TextureTutorial.h"
#include "NormalMappingTutorial.h"
#include "TesselationTutorial.h"
#include "AnimationTutorial.h"
#include "ShadowMappingTutorial.h"

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	Application* example = new ShadowMappingTutorial();

	if (example->create("AIE - Assessment", 1024, 768, argc, argv) == true)
		example->run();

	delete example;

	return 0;
}