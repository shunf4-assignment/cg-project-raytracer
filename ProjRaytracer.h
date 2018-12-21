#pragma once
#include "common/common.h"

class ProjRaytracer : public GLProject
{
public:
	GLuint sceneTex;
	GLuint texOutput;

	double rayTimer;
	double prevTime;
	double currTime;
	double timeDelta;

	glm::mat4 Model;
	glm::mat4 View;
	glm::mat4 Projection;

	GLfloat cameraLong = 0.0;
	GLfloat cameraLati = glm::pi<GLfloat>() / 2 - glm::pi<GLfloat>() / 9;
	GLfloat cameraScaleLog = 2.0;
	GLfloat cameraLongSpeed = glm::pi<GLfloat>() / 6;
	GLfloat cameraLatiSpeed = glm::pi<GLfloat>() / 6;
	GLfloat cameraScaleLogSpeed = 1.0f;
	GLuint balls = 9;

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
	void moveCamera(double timeDelta);
	void progDrawObjects(GLuint progId);
	void progRaytrace(GLuint progId);
	void checkWorkingGroup();

	ProjRaytracer();
	void mainLoop();
	void run();
	~ProjRaytracer();
};