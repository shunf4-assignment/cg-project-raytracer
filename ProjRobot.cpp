#include "ProjRobot.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/polar_coordinates.hpp>
static const GLfloat sqrt3oversqrt2 = 1.22474f;
static const GLfloat sqrt3 = 1.73205f;

ProjRobot::ProjRobot()
{
	//this->GLProject::GLProject();
	this->height = 1024;
	this->width = 1024;
	this->isFullscreen = false;
	this->windowTitle = "Robot Program";
}

void ProjRobot::mainLoop()
{
	while (!glfwWindowShouldClose(windowP))
	{
		static double prevTime = glfwGetTime();
		double currTime = glfwGetTime();
		double timeDelta = currTime - prevTime;
		prevTime = currTime;

		glfwPollEvents();
		moveCamera(timeDelta);
		updateFPSonTitle();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		// 清除颜色缓冲和深度缓冲（启动深度测试时必做）
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (auto prog : programs)
		{
			glUseProgram(prog->id);
			if (prog->func != nullptr)
			{
				(this->*(prog->func))(prog->id);
			}
		}
		glfwSwapBuffers(windowP);
	}
}

void ProjRobot::keyCallbackFunc(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void ProjRobot::doGLConfigure()
{
	// 启用深度测试（ZJQ上课讲过）
	glEnable(GL_DEPTH_TEST);
	// 对每个多边形的正面启用线框绘制模式
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 启动背面剔除
	//glEnable(GL_CULL_FACE);
	// 把每个多边形的背面剔除
	glCullFace(GL_BACK);
	// 设定 "顺时针" 为正方向 (左手定则, 四指指向顺时针方向, 拇指指向朝外方向)
	// glFrontFace(GL_CW);
}

void ProjRobot::doGLFWInit()
{
	logFileP->log("Starting GLFW %s", glfwGetVersionString());
	glfwSetErrorCallback([](int error, const char *desc) -> void {logFileP->error("GLFW Error: [%i] %s", error, desc); });

	if (!glfwInit())
	{
		throw MyGLException("Failed to initialize GLFW");
	}
}

void ProjRobot::doGLEWInit()
{
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		throw MyGLException("Failed to initialize GLEW");
	}
}

void ProjRobot::doCreateWindow()
{
	monitorP = glfwGetPrimaryMonitor();
	const GLFWvidmode* vmode = glfwGetVideoMode(monitorP);
	width = isFullscreen ? vmode->width : width;
	height = isFullscreen ? vmode->height : height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 8);	// 抗锯齿

	windowP = glfwCreateWindow(width, height, windowTitle.c_str(), isFullscreen ? monitorP : nullptr, nullptr);

	if (windowP == nullptr)
	{
		throw MyGLException("Failed to create GLFW window");
	}

	glfwSetWindowSizeCallback(windowP, [](GLFWwindow *window, int width, int height) { glViewport(0, 0, width > height ? height : width, width > height ? height : width); });

	
	glfwMakeContextCurrent(windowP);
	glfwSetKeyCallback(windowP, ProjRobot::keyCallbackFunc);
}

void ProjRobot::doSetViewport()
{
	glfwGetFramebufferSize(windowP, &width, &height);
	glViewport(0, 0, width > height ? height : width, width > height ? height : width);
}

void ProjRobot::doMakePrograms()
{
	GLSLProgram *prog1 = nullptr;

		prog1 = new GLSLProgram((&ProjRobot::progDrawObjects), { {GL_VERTEX_SHADER, "ProjRobot.vert"}, {GL_FRAGMENT_SHADER, "ProjRobot.frag"} });

		prog1->link();
	
	this->programs.push_back(prog1);
}

void ProjRobot::moveCamera(double timeDelta)
{
	// control keys
	bool cam_moved = false;
	bool joint_moved = false;
	if (glfwGetKey(windowP, GLFW_KEY_A)) {
		if (selectedJoint == -1)
		{
			cameraPosition[0] -= static_cast<GLfloat>(cameraSpeed * timeDelta);
			cam_moved = true;
		}
		else
		{
			jointPosition[selectedJoint][0] -= static_cast<GLfloat>(jointSpeed * timeDelta);
			joint_moved = true;
		}
	}
	if (glfwGetKey(windowP, GLFW_KEY_D)) {
		if (selectedJoint == -1)
		{
			cameraPosition[0] += static_cast<GLfloat>(cameraSpeed * timeDelta);
			cam_moved = true;
		}
		else
		{
			jointPosition[selectedJoint][0] += static_cast<GLfloat>(jointSpeed * timeDelta);
			joint_moved = true;
		}
	}
	if (glfwGetKey(windowP, GLFW_KEY_W)) {
		if (selectedJoint == -1)
		{
			cameraPosition[1] += static_cast<GLfloat>(cameraSpeed * timeDelta);
			cam_moved = true;
		}
		else
		{
			jointPosition[selectedJoint][1] -= static_cast<GLfloat>(jointSpeed * timeDelta);
			joint_moved = true;
		}
	}
	if (glfwGetKey(windowP, GLFW_KEY_S)) {
		if (selectedJoint == -1)
		{
			cameraPosition[1] -= static_cast<GLfloat>(cameraSpeed * timeDelta);
		cam_moved = true;
		}
		else
		{
			jointPosition[selectedJoint][1] += static_cast<GLfloat>(jointSpeed * timeDelta);
			joint_moved = true;
		}
	}
	if (glfwGetKey(windowP, GLFW_KEY_PAGE_UP)) {
		if (selectedJoint == -1)
		{
			cameraPosition[2] -= static_cast<GLfloat>(cameraSpeed * timeDelta);
			cam_moved = true;
		}
		else
		{
			jointPosition[selectedJoint][2] -= static_cast<GLfloat>(jointSpeed * timeDelta);
			joint_moved = true;
		}
	}
	if (glfwGetKey(windowP, GLFW_KEY_PAGE_DOWN)) {
		if (selectedJoint == -1)
		{
			cameraPosition[2] += static_cast<GLfloat>(cameraSpeed * timeDelta);
		cam_moved = true;
		}
		else
		{
			jointPosition[selectedJoint][2] += static_cast<GLfloat>(jointSpeed * timeDelta);
			joint_moved = true;
		}
	}
	if (glfwGetKey(windowP, GLFW_KEY_LEFT)) {
		cameraYaw += static_cast<GLfloat>(cameraYawSpeed * timeDelta);
		cam_moved = true;
	}
	if (glfwGetKey(windowP, GLFW_KEY_RIGHT)) {
		cameraYaw -= static_cast<GLfloat>(cameraYawSpeed * timeDelta);
		cam_moved = true;
	}
	if (glfwGetKey(windowP, GLFW_KEY_UP)) {
		cameraPitch += static_cast<GLfloat>(cameraPitchSpeed * timeDelta);
		cam_moved = true;
	}
	if (glfwGetKey(windowP, GLFW_KEY_DOWN)) {
		cameraPitch -= static_cast<GLfloat>(cameraPitchSpeed * timeDelta);
		cam_moved = true;
	}

	static bool tabPressed = false;

	if (glfwGetKey(windowP, GLFW_KEY_TAB) == GLFW_PRESS) {
		tabPressed = true;
	}
	else 
	{
		if (tabPressed)
		{
			selectedJoint += 1;
			
			if (selectedJoint == 12)
			{
				selectedJoint = -1;
			}

			if (selectedJoint % 3 == 0)
				selectedJoint++;


			lg("Selected Joint: %d", selectedJoint);
			updateJointColor();
			tabPressed = false;

		}
	}
	// update view matrix
	if (cam_moved) {
		View = glm::translate(glm::mat4(1), -glm::make_vec3(cameraPosition));
		View *= glm::rotate(glm::mat4(1), glm::radians(cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f));
		View *= glm::rotate(glm::mat4(1), glm::radians(cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	if (joint_moved)
	{
		updateLimbPosition();
	}
}

void ProjRobot::updateFPSonTitle()
{
	static double startTime = glfwGetTime();
	static int frameCounter = 0;
	double currTime = glfwGetTime();
	if (currTime == 0)
	{
		throw MyGLException("glfwGetTime() 发生错误");
	}

	double delta = currTime - startTime;
	frameCounter++;
	if (delta >= 0.25)
	{
		// 距离上次更新已经过去了 0.25s, 再次更新标题栏
		startTime = currTime;
		char newTitle[100];
		sprintf_s(newTitle, "%s @ FPS : %.2f", windowTitle.c_str(), static_cast<double>(frameCounter) / delta);
		glfwSetWindowTitle(windowP, newTitle);
		frameCounter = 0;
	}
}

void ProjRobot::makePyramid(GLfloat peakx, GLfloat peaky, GLfloat peakz, GLfloat height)
{
	GLfloat edge = height * sqrt3oversqrt2;
	GLfloat verticesAndColors[] = {
		// 顶点x，顶点y，顶点z，顶点颜色R，顶点颜色G，顶点颜色B
		// 这里由于不同面的顶点颜色不同，所以同一位置的顶点也要写三次。如果颜色统一，可以只写一次，之后多次使用到这个顶点时使用 EBO 索引
		peakx, peaky, peakz, 0.3f, 0.0f, 0.0f,
		peakx, peaky, peakz, 0.0f, 0.3f, 0.0f,
		peakx, peaky, peakz, 0.0f, 0.0f, 0.0f,
		peakx - edge / 2, peaky - edge / sqrt3 / 2, peakz - height, 0.3f, 0.0f, 0.0f,
		peakx - edge / 2, peaky - edge / sqrt3 / 2, peakz - height, 0.0f, 0.3f, 0.0f,
		peakx - edge / 2, peaky - edge / sqrt3 / 2, peakz - height, 0.0f, 0.0f, 0.0f,
		peakx + edge / 2, peaky - edge / sqrt3 / 2, peakz - height, 0.0f, 0.3f, 0.0f,
		peakx + edge / 2, peaky - edge / sqrt3 / 2, peakz - height, 0.0f, 0.0f, 0.3f,
		peakx + edge / 2, peaky - edge / sqrt3 / 2, peakz - height, 0.0f, 0.0f, 0.0f,
		peakx, peaky + edge / sqrt3, peakz - height, 0.3f, 0.0f, 0.0f,
		peakx, peaky + edge / sqrt3, peakz - height, 0.0f, 0.0f, 0.3f,
		peakx, peaky + edge / sqrt3, peakz - height, 0.0f, 0.0f, 0.0f
	};

	/* 索引
		上述数组经过 glVertexAttribPointer 描述后产生的顶点着色器中的两个数组 position(0号位置) 和 color(1号位置) 描述了画一个三棱锥（四个三角形，十二个顶点）所需要的所有位置和颜色信息。每次，使用 position[i] 和 color[i] 画第 i 个顶点。
		这里的 indices 即是用索引的方式描述四个三角形的描绘顺序，如 0 9 3 表示红色的一个三角形。
	*/
	// 注意三角形顶点的绘制顺序与三角形的正面朝向有关系(左手定则)
	GLuint indices[] = {
		0, 9, 3,
		1, 4, 6,
		2, 7, 10,
		5, 11, 8
	};

	VAO_ext obj;
	glGenVertexArrays(1, &obj.vao);
	obj.mode = GL_TRIANGLES;
	obj.count = 12;
	obj.type = GL_UNSIGNED_INT;
	obj.indices = nullptr;
	obj.Model = glm::mat4(1.0f);
	vaos.push_back(obj);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	buffers.push_back(vbo);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	buffers.push_back(ebo);

	// 绑定 VAO 对象到当前 vao，开始定义这个 vao 的内容。
	glBindVertexArray(obj.vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesAndColors), verticesAndColors, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const void *)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int ProjRobot::makeCylinder(GLfloat length, GLfloat radius, Color(*getColor)(GLfloat theta, CylinderSurface cyFace), glm::vec3 topCenter, GLfloat rotateAngle, glm::vec3 rotateAxis)
{
	// 画一个沿 z 轴负方向的, 长度为 length, 粗度为 radius * 2 的 圆柱
	// 上下顶盖 + 侧边
	GLfloat verticesAndColors[(4 * cylinderFineness + 2) * 6];
	GLuint indices[12 * cylinderFineness];

	verticesAndColors[6 * cylinderFineness * 4 + 0] = 0.0f;
	verticesAndColors[6 * cylinderFineness * 4 + 1] = 0.0f;
	verticesAndColors[6 * cylinderFineness * 4 + 2] = 0.0f;
	reinterpret_cast<Color &>(verticesAndColors[6 * cylinderFineness * 4 + 3]) = getColor(0.0f, CylinderTopCenter);

	verticesAndColors[6 * cylinderFineness * 4 + 6 + 0] = 0.0f;
	verticesAndColors[6 * cylinderFineness * 4 + 6 + 1] = 0.0f;
	verticesAndColors[6 * cylinderFineness * 4 + 6 + 2] = -1.0f;
	reinterpret_cast<Color &>(verticesAndColors[6 * cylinderFineness * 4 + 6 + 3]) = getColor(0.0f, CylinderBottomCenter);

	GLfloat step = glm::radians(360.0f) / cylinderFineness;

	GLfloat theta = 0.0f;

	for (int i = 0; i < cylinderFineness; i++, theta += step)
	{
		GLfloat x = glm::cos(theta) * radius;
		GLfloat y = glm::sin(theta) * radius;
		verticesAndColors[6 * cylinderFineness * CylinderTop + i * 6 + 0] = x;
		verticesAndColors[6 * cylinderFineness * CylinderTop + i * 6 + 1] = y;
		verticesAndColors[6 * cylinderFineness * CylinderTop + i * 6 + 2] = 0.0f;
		*reinterpret_cast<Color *>(&verticesAndColors[6 * cylinderFineness * CylinderTop + i * 6 + 3]) = getColor(theta, CylinderTop);

		verticesAndColors[6 * cylinderFineness * CylinderSide_top + i * 6 + 0] = x;
		verticesAndColors[6 * cylinderFineness * CylinderSide_top + i * 6 + 1] = y;
		verticesAndColors[6 * cylinderFineness * CylinderSide_top + i * 6 + 2] = 0.0f;
		*reinterpret_cast<Color *>(&verticesAndColors[6 * cylinderFineness * CylinderSide_top + i * 6 + 3]) = getColor(theta, CylinderSide_top);

		verticesAndColors[6 * cylinderFineness * CylinderSide_bottom + i * 6 + 0] = x;
		verticesAndColors[6 * cylinderFineness * CylinderSide_bottom + i * 6 + 1] = y;
		verticesAndColors[6 * cylinderFineness * CylinderSide_bottom + i * 6 + 2] = -1.0f;
		*reinterpret_cast<Color *>(&verticesAndColors[6 * cylinderFineness * CylinderSide_bottom + i * 6 + 3]) = getColor(theta, CylinderSide_bottom);

		verticesAndColors[6 * cylinderFineness * CylinderBottom + i * 6 + 0] = x;
		verticesAndColors[6 * cylinderFineness * CylinderBottom + i * 6 + 1] = y;
		verticesAndColors[6 * cylinderFineness * CylinderBottom + i * 6 + 2] = -1.0f;
		*reinterpret_cast<Color *>(&verticesAndColors[6 * cylinderFineness * CylinderBottom + i * 6 + 3]) = getColor(theta, CylinderBottom);
		
		// 第一个三角形
		indices[i * 12 + 0] = cylinderFineness * 4; // TopCenter
		indices[i * 12 +  1 ] = cylinderFineness * 0 + i;
		indices[i * 12 +  2 ] = cylinderFineness * 0 + (i + 1) % cylinderFineness;

		// 第二个三角形
		indices[i * 12 + 3 + 0] = cylinderFineness * 1 + i;
		indices[i * 12 + 3 +  1 ] = cylinderFineness * 2 + i;
		indices[i * 12 + 3 +  2 ] = cylinderFineness * 1 + (i + 1) % cylinderFineness;

		// 第三个三角形
		indices[i * 12 + 6 + 0] = cylinderFineness * 2 + i;
		indices[i * 12 + 6 + 2] = cylinderFineness * 1 + (i + 1) % cylinderFineness;
		indices[i * 12 + 6 + 1] = cylinderFineness * 2 + (i + 1) % cylinderFineness;

		// 第四个三角形
		indices[i * 12 + 9 + 0] = cylinderFineness * 4 + 1; // BottomCenter
		indices[i * 12 + 9 + 2] = cylinderFineness * 3 + i;
		indices[i * 12 + 9 + 1] = cylinderFineness * 3 + (i + 1) % cylinderFineness;
	}

	VAO_ext obj;
	glGenVertexArrays(1, &obj.vao);
	obj.mode = GL_TRIANGLES;
	obj.count = cylinderFineness * 4 * 3;
	obj.type = GL_UNSIGNED_INT;
	obj.indices = nullptr;
	obj.Model = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), topCenter), rotateAngle, rotateAxis), { 1.0f, 1.0f, length });
	vaos.push_back(obj);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	buffers.push_back(vbo);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	buffers.push_back(ebo);

	// 绑定 VAO 对象到当前 vao，开始定义这个 vao 的内容。
	glBindVertexArray(obj.vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesAndColors), verticesAndColors, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const void *)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return vaos.size() - 1;
}

glm::vec3 spherical2Euclidean(const glm::vec3 &spherical)
{
	return { 
		spherical[0] * glm::sin(spherical[2]) * glm::cos(spherical[1]),
		spherical[0] * glm::sin(spherical[2]) * glm::sin(spherical[1]),
		spherical[0] * glm::cos(spherical[2])
	};
}

int ProjRobot::makeSphere(glm::vec3 center, GLfloat radius, Color(*getColor)(glm::vec3 sphCoord))
{
	GLfloat verticesAndColors[((sphereStacks - 1) * sphereSlices + 2) * 6] = { 0 };
	GLuint indices[6 * (sphereStacks - 1) * sphereSlices] = { 0 };

	glm::vec3 coord = spherical2Euclidean({ radius, 0.0f, 0.0f });
	// 北极
	verticesAndColors[((sphereStacks - 1) * sphereSlices) * 6 + 0] = coord[0];
	verticesAndColors[((sphereStacks - 1) * sphereSlices) * 6 + 1] = coord[1];
	verticesAndColors[((sphereStacks - 1) * sphereSlices) * 6 + 2] = coord[2];
	reinterpret_cast<Color &>(verticesAndColors[((sphereStacks - 1) * sphereSlices) * 6 + 3]) = getColor({radius, 0.0f, 0.0f});

	coord = spherical2Euclidean({ radius, 0.0f, glm::pi<GLfloat>() });
	// 南极
	verticesAndColors[((sphereStacks - 1) * sphereSlices) * 6 + 6 + 0] = coord[0];
	verticesAndColors[((sphereStacks - 1) * sphereSlices) * 6 + 6 + 1] = coord[1];
	verticesAndColors[((sphereStacks - 1) * sphereSlices) * 6 + 6 + 2] = coord[2];
	reinterpret_cast<Color &>(verticesAndColors[((sphereStacks - 1) * sphereSlices) * 6 + 6 + 3]) = getColor({ radius, 0.0f, glm::pi<GLfloat>() });

	GLfloat phiStep = glm::radians(360.0f) / sphereSlices;
	GLfloat thetaStep = glm::radians(180.0f) / sphereStacks;

	GLfloat phi = 0.0f;

	for (int i = 0; i < sphereSlices; i++, phi += phiStep)
	{
		GLfloat x = glm::cos(phi) * radius;
		GLfloat y = glm::sin(phi) * radius;

		// 第一个三角形
		indices[i * 6 * (sphereStacks - 1) + 0] = (sphereStacks - 1) * sphereSlices; // 北极
		indices[i * 6 * (sphereStacks - 1) + 1] = i * (sphereStacks - 1);
		indices[i * 6 * (sphereStacks - 1) + 2] = ((i + 1) % sphereSlices) * (sphereStacks - 1);

		GLfloat theta = thetaStep;
		
		for (int j = 0; j < sphereStacks - 1; j++, theta += thetaStep)
		{
			glm::vec3 coord = spherical2Euclidean({ radius, phi, theta });
			verticesAndColors[6 * (i * (sphereStacks - 1) + j) + 0] = coord[0];
			verticesAndColors[6 * (i * (sphereStacks - 1) + j) + 1] = coord[1];
			verticesAndColors[6 * (i * (sphereStacks - 1) + j) + 2] = coord[2];
			*reinterpret_cast<Color *>(&verticesAndColors[6 * (i * (sphereStacks - 1) + j) + 3]) = getColor({ radius, phi, theta });
			
			if (j != 0)
			{
				indices[i * 6 * (sphereStacks - 1) + 3 + (j - 1) * 6 + 0] = i * (sphereStacks - 1) + (j - 1);
				indices[i * 6 * (sphereStacks - 1) + 3 + (j - 1) * 6 + 1] = i * (sphereStacks - 1) + j;
				indices[i * 6 * (sphereStacks - 1) + 3 + (j - 1) * 6 + 2] = (i + 1) % sphereSlices * (sphereStacks - 1) + (j - 1);

				indices[i * 6 * (sphereStacks - 1) + 3 + (j - 1) * 6 + 3] = i * (sphereStacks - 1) + j;
				indices[i * 6 * (sphereStacks - 1) + 3 + (j - 1) * 6 + 4] = (i + 1) % sphereSlices * (sphereStacks - 1) + j;
				indices[i * 6 * (sphereStacks - 1) + 3 + (j - 1) * 6 + 5] = (i + 1) % sphereSlices * (sphereStacks - 1) + (j - 1);
			}
			
		}
		
		
		// 最后一个三角形
		indices[(i + 1) * 6 * (sphereStacks - 1) - 3] = (sphereStacks - 1) * sphereSlices + 1; // 南极
		indices[(i + 1) * 6 * (sphereStacks - 1) -  1 ] = i * (sphereStacks - 1) + (sphereStacks - 2);
		indices[(i + 1) * 6 * (sphereStacks - 1) -  2 ] = ((i + 1) % sphereSlices) * (sphereStacks - 1) + (sphereStacks - 2);
	}

	VAO_ext obj;
	glGenVertexArrays(1, &obj.vao);
	obj.mode = GL_TRIANGLES;
	obj.count = sizeof(indices) / sizeof(indices[0]);
	obj.type = GL_UNSIGNED_INT;
	obj.indices = nullptr;
	obj.Model = glm::translate(glm::mat4(1.0f), center);
	vaos.push_back(obj);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	buffers.push_back(vbo);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	buffers.push_back(ebo);

	// 绑定 VAO 对象到当前 vao，开始定义这个 vao 的内容。
	glBindVertexArray(obj.vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesAndColors), verticesAndColors, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const void *)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return vaos.size() - 1;
}

int ProjRobot::makeCylinder(glm::vec3 topCenter, glm::vec3 bottomCenter, GLfloat radius, Color(*getColor)(GLfloat theta, CylinderSurface cyFace))
{
	glm::vec3 minusZ(0.0f, 0.0f, -1.0f);
	glm::vec3 top2Bottom = glm::normalize(bottomCenter - topCenter);

	GLfloat angle = glm::angle(minusZ, top2Bottom);
	glm::vec3 axis = glm::cross(minusZ, top2Bottom);
	return makeCylinder(glm::distance(topCenter, bottomCenter), radius, getColor, topCenter, angle, axis);
}

void updateCylinder(VAO_ext &cylinder, glm::vec3 topCenter, glm::vec3 bottomCenter )
{
	GLfloat length = glm::distance(topCenter, bottomCenter);
	glm::vec3 minusZ(0.0f, 0.0f, -1.0f);
	glm::vec3 top2Bottom = glm::normalize(bottomCenter - topCenter);

	GLfloat angle = glm::angle(minusZ, top2Bottom);
	glm::vec3 axis = glm::cross(minusZ, top2Bottom);
	cylinder.Model = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), topCenter), angle, axis), { 1.0f, 1.0f, length });
}

void updateSphere(VAO_ext &sphere, glm::vec3 center)
{
	sphere.Model = glm::translate(glm::mat4(1.0f), center);
}


void ProjRobot::makeMatrices()
{
	const GLfloat transArr[16] = {
		1.0f, 0.0f, 0.0f, 0.0f, // first column 
		0.0f, 1.0f, 0.0f, 0.0f, // second column 
		0.0f, 0.0f, 1.0f, 0.0f, // third column 
		0.0f, 0.0f, 0.0f, 1.0f // fourth column 
	};
	
	//View = glm::lookAt(glm::vec3(0.0f, 0.0f, 100.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0, 1, 0));
	View = glm::translate(glm::mat4(1), -glm::make_vec3(cameraPosition));
	View *= glm::rotate(glm::mat4(1), glm::radians(cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f));
	View *= glm::rotate(glm::mat4(1), glm::radians(cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f));

	//Projection = glm::ortho(-50.f, 50.f, -50.f, 50.f, 0.f, 60.f);
	Projection = glm::perspective(glm::radians(45.0f), 1.0f / 1.0f, 0.3f, 1000.0f);
}

void ProjRobot::progDrawObjects(GLuint progId)
{
	GLint modelLoc = glGetUniformLocation(progId, "Model");
	GLint viewLoc = glGetUniformLocation(progId, "View");
	GLint projectionLoc = glGetUniformLocation(progId, "Projection");
	GLint dimLoc = glGetUniformLocation(progId, "dim");

	glUniformMatrix4fv(viewLoc, 1, false, glm::value_ptr(View));
	glUniformMatrix4fv(projectionLoc, 1, false, glm::value_ptr(Projection));

	for (auto p = vaos.begin(); p != vaos.end(); p++)
	{
		glUniformMatrix4fv(modelLoc, 1, false, glm::value_ptr(p->Model));
		glUniform1f(dimLoc, (p->dim));

		glBindVertexArray(p->vao);
		glDrawElements(p->mode, p->count, p->type, p->indices);
	}
	glBindVertexArray(0);
}

void ProjRobot::updateJointColor()
{
	for (int i = 0; i < 12; i++)
	{
		//if ((i + 1) % 3 )
			if (selectedJoint == i)
				vaos[jointVAO[i]].dim = 1.0f;
			else 
				vaos[jointVAO[i]].dim = 0.5f;
	}
}

void ProjRobot::updateLimbPosition()
{
	updateCylinder(vaos[arm11], jointPosition[0], jointPosition[1]);
	updateCylinder(vaos[arm12], jointPosition[1], jointPosition[2]);
	updateCylinder(vaos[arm21], jointPosition[3], jointPosition[4]);
	updateCylinder(vaos[arm22], jointPosition[4], jointPosition[5]);
	updateCylinder(vaos[leg11], jointPosition[6], jointPosition[7]);
	updateCylinder(vaos[leg12], jointPosition[7], jointPosition[8]);
	updateCylinder(vaos[leg21], jointPosition[9], jointPosition[10]);
	updateCylinder(vaos[leg22], jointPosition[10], jointPosition[11]);

	for (int i = 0; i < 12; i++)
	{
		if (true)
		{
			updateSphere(vaos[jointVAO[i]], jointPosition[i]);
		}
	}
	
}

void ProjRobot::doInit()
{
	doGLFWInit();
	doCreateWindow();
	doGLEWInit();
	printGLParams(logFileP);
	doGLConfigure();
	doSetViewport();
}

static Color HSV2RGB(GLfloat h, GLfloat s, GLfloat v)
{
	float       hh, p, q, t, ff;
	long        i;

	if (s <= 0.0) {       // < is bogus, just shuts up warnings
		return Color{v, v, v};
	}
	hh = glm::degrees(h);
	if (hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = v * (1.0f - s);
	q = v * (1.0f - (s * ff));
	t = v * (1.0f - (s * (1.0f - ff)));

	switch (i) {
	case 0:
		return Color{ v, t, p };
		break;
	case 1:
		return Color{ q, v, p };
		break;
	case 2:
		return Color{ p, v, t };
		break;

	case 3:
		return Color{ p, q, v };
		break;
	case 4:
		return Color{ t, p, v };
		break;
	case 5:
	default:
		return Color{ v, p, q };
		break;
	}
}

static Color cylinderColor(GLfloat radian, CylinderSurface cyFace)
{
	switch (cyFace)
	{
	case CylinderTopCenter:
	case CylinderBottomCenter:
		return Color{ 1.0f, 1.0f, 1.0f };
	case CylinderTop:
		return HSV2RGB(radian, 1.0, 1.0);
	case CylinderBottom:
		return HSV2RGB(2 * glm::pi<GLfloat>() - radian, 1.0, 1.0);
	case CylinderSide_top:
		return HSV2RGB(radian, 1.0, 1.0);
	default:
		return HSV2RGB(2 * glm::pi<GLfloat>() - radian, 1.0, 1.0);
	}
}

static Color cylinderColor2_body(GLfloat radian, CylinderSurface cyFace)
{
	switch (cyFace)
	{
	case CylinderTopCenter:
	case CylinderBottomCenter:
	case CylinderTop:
	case CylinderBottom:
		return Color{ 0.0f, 0.0f, 0.3f };
	
	case CylinderSide_top:
	default:
		return Color{ 0.2f, 0.2f, 0.4f };
	}
}

static Color cylinderColor2_limb(GLfloat radian, CylinderSurface cyFace)
{
	switch (cyFace)
	{
	case CylinderTopCenter:
	case CylinderBottomCenter:
	case CylinderTop:
	case CylinderBottom:
		return Color{ 0.0f, 0.0f, 0.5f };

	case CylinderSide_top:
	default:
		return Color{ 0.3f, 0.3f, 0.7f };
	}
}

static Color sphereColor(glm::vec3 sphCoord)
{
	return HSV2RGB(sphCoord[1], 1.0, 1.0);
}

void ProjRobot::makeRobot()
{
	selectedJoint = -1;
	jointPosition[0] = { -10.0f, 15.0f, 0.0f };
	jointPosition[1] = { -18.0f, 3.0f, 0.0f };
	jointPosition[2] = { -22.0f, -8.0f, 0.0f };
	jointPosition[3] = { 10.0f, 15.0f, 0.0f };
	jointPosition[4] = { 18.0f, 3.0f, 0.0f };
	jointPosition[5] = { 22.0f, -8.0f, 0.0f };
	jointPosition[6] = { -9.0f, -15.0f, 0.0f };
	jointPosition[7] = { -9.0f, -25.0f, 0.0f };
	jointPosition[8] = { -9.0f, -40.0f, 0.0f };
	jointPosition[9] = { 9.0f, -15.0f, 0.0f };
	jointPosition[10] = { 9.0f, -25.0f, 0.0f };
	jointPosition[11] = { 9.0f, -40.0f, 0.0f };

	makeSphere({ 0.0f, 22.0f, 0.0f }, 10.0f, [](glm::vec3 sphCoord) -> Color {
		return { .3f,0.0f,0.0f };
	});
	makeCylinder({ 0.0f, 15.0f, 0.0f }, { 0.0f, -15.0f, 0.0f }, 12.0f, cylinderColor2_body);

	GLfloat armThickness = 3.0f;
	GLfloat legThickness = 5.0f;

	for (int i = 0; i < 12; i++)
	{
		if (true)
		{
			jointVAO[i] = makeSphere(jointPosition[i], i < 6 ? 3.5f : 5.5f, [](glm::vec3 sphCoord) -> Color {
				return { .9f,0.0f,0.0f };
			});
			vaos[jointVAO[i]].dim = 0.5f;
		}
	}

	arm11 = makeCylinder(jointPosition[0], jointPosition[1], armThickness, cylinderColor2_limb);
	arm12 = makeCylinder(jointPosition[1], jointPosition[2], armThickness, cylinderColor2_limb);

	arm21 = makeCylinder(jointPosition[3], jointPosition[4], armThickness, cylinderColor2_limb);
	arm22 = makeCylinder(jointPosition[4], jointPosition[5], armThickness, cylinderColor2_limb);

	leg11 = makeCylinder(jointPosition[6], jointPosition[7], legThickness, cylinderColor2_limb);
	leg12 = makeCylinder(jointPosition[7], jointPosition[8], legThickness, cylinderColor2_limb);

	leg21 = makeCylinder(jointPosition[9], jointPosition[10], legThickness, cylinderColor2_limb);
	leg22 = makeCylinder(jointPosition[10], jointPosition[11], legThickness, cylinderColor2_limb);
	
}

void ProjRobot::run()
{
	doInit();
	doMakePrograms();
	//makePyramid(0.0f, 0.0f, 0.0f, 50.0f);
	//makeCylinder({ 0.0f, 0.0f, 0.0f }, { 15.0f, 20.0f, -25.0f }, 5.0f, cylinderColor);
	//makeCylinder({ 0.0f, 0.0f, 0.0f }, { -15.0f, -20.0f, 25.0f }, 5.0f, cylinderColor);
	//makeSphere({ 0.0f, 0.0f, 0.0f }, 20.0f, sphereColor);
	makeRobot();
	makeMatrices();
	std::cout << "Press TAB to switch between Joints (when -1, move the camera). WASD and PageUp and PageDown to move the camera or joints. Up/Down/Left/Right to rotate the camera." << std::endl;
	mainLoop();
}


ProjRobot::~ProjRobot()
{
	for (auto p : programs)
	{
		delete p;
	}

	for (auto p = vaos.begin(); p != vaos.end(); p++)
	{
		glDeleteVertexArrays(1, &((*p).vao));
	}

	for (auto p = buffers.begin(); p != buffers.end(); p++)
	{
		glDeleteBuffers(1, &*p);
	}

	glfwTerminate();
}
