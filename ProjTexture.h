#pragma once
#include "common/common.h"

class ProjTexture :
	public GLProject
{
public:
	GLImage *image;

	glm::mat4 Model;
	glm::mat4 View;
	glm::mat4 Projection;

	static void keyCallbackFunc(GLFWwindow *window, int key, int scancode, int action, int mode);
	void doGLConfigure();
	void doGLFWInit();
	void doGLEWInit();
	void doInit();
	void doCreateWindow();
	void doSetViewport();
	void doMakePrograms();
	void updateFPSonTitle();
	void makeRectangle();
	void makeTexture();
	void makeMatrices();
	void progDrawObjects(GLuint progId);

	ProjTexture();
	void mainLoop();
	void run();
	~ProjTexture();
};

