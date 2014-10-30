#include "AdvancedTextureTutorial.h"
#include "TextureTutorial.h"
#include "NormalMappingTutorial.h"
#include "TesselationTutorial.h"
#include "AnimationTutorial.h"
#include "Tutorial12_Shadows.h"
#include "Tutorial7.h"
// main that controls the creation/destruction of an application

int main(int argc, char* argv[])
{
	Application * texturing = new TextureTutorial();

	if (texturing->create("AIE - Assessment", 1024, 768, argc, argv) == true)
		texturing->run();

	delete texturing;

	Application * tesselation = new TesselationTutorial();

	if (tesselation->create("AIE - Assessment", 1024, 768, argc, argv) == true)
		tesselation->run();

	delete tesselation;

	Application * animation = new AnimationTutorial();

	if (animation->create("AIE - Assessment", 1024, 768, argc, argv) == true)
		animation->run();

	delete animation;

	Application * particles = new Tutorial7();

	if (particles->create("AIE - Assessment", 1024, 768, argc, argv) == true)
		particles->run();

	delete particles;

	Application* shadows = new Tutorial12_Shadows();

	if (shadows->create("AIE - Assessment", 1024, 768, argc, argv) == true)
		shadows->run();

	delete shadows;



	return 0;
}