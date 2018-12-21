#pragma once
#include "common/common.h"

const GLuint cylinderFineness = 100;
const GLuint sphereSlices = 100;
const GLuint sphereStacks = 100;

class VAO_ext : public VAOWithDrawParam
{
public:
	GLfloat dim = 1.0f;
};

struct Color
{
	GLfloat r;
	GLfloat g;
	GLfloat b;
};

enum CylinderSurface
{
	CylinderTop = 0,
	CylinderSide_top = 1,
	CylinderSide_bottom = 2,
	CylinderBottom = 3,
	CylinderTopCenter = 4,
	CylinderBottomCenter = 5
};

class ProjRobot :
	public GLProject
{
public:
	//glm::mat4 Model;
	glm::mat4 View;
	glm::mat4 Projection;

	int selectedJoint;
	glm::vec3 jointPosition[12];
	int jointVAO[12];

	std::vector< VAO_ext> vaos;

	int arm11;
	int arm12;
	int arm21;
	int arm22;

	int leg11;
	int leg12;
	int leg21;
	int leg22;


	GLfloat cameraPosition[3] = { 0.0f, 0.0f, 50.0f };
	GLfloat cameraYaw = 0.0f;
	GLfloat cameraPitch = 0.0f;
	GLfloat cameraSpeed = 20.0f;
	GLfloat jointSpeed = 7.0f;
	GLfloat cameraYawSpeed = 20.0f;
	GLfloat cameraPitchSpeed = 20.0f;

	static void keyCallbackFunc(GLFWwindow *window, int key, int scancode, int action, int mode);
	void doGLConfigure();
	void doGLFWInit();
	void doGLEWInit();
	void doInit();
	void doCreateWindow();
	void doSetViewport();
	void doMakePrograms();
	void moveCamera(double timeDelta);
	void updateFPSonTitle();
	void makePyramid(GLfloat peakx, GLfloat peaky, GLfloat peakz, GLfloat height);
	int makeCylinder(GLfloat length, GLfloat radius, Color (*getColor)(GLfloat theta, CylinderSurface cyFace), glm::vec3 topCenter, GLfloat rotateAngle, glm::vec3 rotateAxis);
	int makeSphere(glm::vec3 center, GLfloat radius, Color color);
	int makeSphere(glm::vec3 center, GLfloat radius, Color(*getColor)(glm::vec3 sphCoord));
	int makeCylinder(glm::vec3 topCenter, glm::vec3 bottomCenter, GLfloat radius, Color(*getColor)(GLfloat theta, CylinderSurface cyFace));
	void makeMatrices();
	void progDrawObjects(GLuint progId);

	void updateJointColor();
	void updateLimbPosition();

	ProjRobot();
	void mainLoop();
	void makeRobot();
	void run();
	~ProjRobot();
};

