#include "common/common.h"
#define FORCE_NVIDIA_GPU
#include "common/headerForMain.h"

#include "ProjRobot.h"
#include "ProjRaytracer.h"
#include "ProjTexture.h"

GLProject *projP;

int main()
{
	ProjRaytracer proj1;

	projP = &proj1;
	projP->run();
	return 0;
}