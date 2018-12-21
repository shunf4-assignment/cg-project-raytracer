#include "ProjRaytracer.h"
using namespace std;

const GLfloat RayComputePeriod = 0.04;

static int initialHeight = 1024;
static int initialWidth = 1024;

ProjRaytracer::ProjRaytracer()
{
	//this->GLProject::GLProject();
	this->height = initialHeight;
	this->width = initialWidth;
	this->isFullscreen = false;
	this->windowTitle = "My Raytracer Program";
	this->rayTimer = 0;
}

void ProjRaytracer::mainLoop()
{
	std::cout << "Use arrow keys, PgUp and PgDn to move the camera. Use [ and ] to inc/dec the number of balls." << std::endl;
	std::cout << "Edit ProjRaytracer.comp, declaring paintNum(the number of balls) to be const but not uniform to raise speed." << std::endl;
	prevTime = glfwGetTime();
	while (!glfwWindowShouldClose(windowP))
	{
		currTime = glfwGetTime();
		timeDelta = currTime - prevTime;
		rayTimer += timeDelta;
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

void ProjRaytracer::keyCallbackFunc(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void ProjRaytracer::doGLConfigure()
{
	// 启用深度测试（ZJQ上课讲过）
	glEnable(GL_DEPTH_TEST);
	// 对每个多边形的正面启用线框绘制模式
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 启动背面剔除
	//glEnable(GL_CULL_FACE);
	// 把每个多边形的背面剔除
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CW);
}

void ProjRaytracer::doGLFWInit()
{
	logFileP->log("Starting GLFW %s", glfwGetVersionString());
	glfwSetErrorCallback([](int error, const char *desc) -> void {logFileP->error("GLFW Error: [%i] %s", error, desc); });

	if (!glfwInit())
	{
		throw MyGLException("Failed to initialize GLFW");
	}
}

void ProjRaytracer::doGLEWInit()
{
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		throw MyGLException("Failed to initialize GLEW");
	}
}

void ProjRaytracer::doCreateWindow()
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

	glfwSetWindowSizeCallback(windowP, [](GLFWwindow *window, int width, int height) { 
		int newWidth = width > height ? height : width;
		int newHeight = width > height ? height : width;
		newWidth = newWidth > 1024 ? 1024 : newWidth;
		newHeight = newHeight > 1024 ? 1024 : newHeight;
		glViewport(0, 0, newWidth, newHeight);
		projP->setWidth(newWidth);
		projP->setHeight(newHeight);
	});


	glfwMakeContextCurrent(windowP);
	glfwSetKeyCallback(windowP, ProjRaytracer::keyCallbackFunc);
}

void ProjRaytracer::doSetViewport()
{
	glfwGetFramebufferSize(windowP, &width, &height);
	//glViewport(0, 0, width > height ? height : width, width > height ? height : width);
	glViewport(0, 0, width, height);
}

void ProjRaytracer::doMakePrograms()
{
	GLSLProgram *prog1 = nullptr;
	GLSLProgram *prog2 = nullptr;

	prog1 = new GLSLProgram(&ProjRaytracer::progDrawObjects, { {GL_VERTEX_SHADER, "ProjRaytracer.vert"}, {GL_FRAGMENT_SHADER, "ProjRaytracer.frag"} });

	prog2 = new GLSLProgram(&ProjRaytracer::progRaytrace, { {GL_COMPUTE_SHADER, "ProjRaytracer.comp"} });

	prog1->link();
	prog2->link();

	prog1->validate();
	prog2->validate();

	prog1->printInfo(logFileP);
	prog2->printInfo(logFileP);

	this->programs.push_back(prog1);
	this->programs.push_back(prog2);
}

void ProjRaytracer::moveCamera(double timeDelta)
{
	// control keys
	bool cam_moved = false;
	
	if (glfwGetKey(windowP, GLFW_KEY_PAGE_UP)) {
		cameraScaleLog -= static_cast<GLfloat>(cameraScaleLogSpeed * timeDelta);
		cam_moved = true;
	}
	if (glfwGetKey(windowP, GLFW_KEY_PAGE_DOWN)) {
		cameraScaleLog += static_cast<GLfloat>(cameraScaleLogSpeed * timeDelta);
		cam_moved = true;
	}
	if (glfwGetKey(windowP, GLFW_KEY_LEFT)) {
		cameraLong -= static_cast<GLfloat>(cameraLongSpeed * timeDelta);
		if (cameraLong < 0.0f) {
			cameraLong += glm::pi<GLfloat>() * 2;
		}
		cam_moved = true;
	}
	if (glfwGetKey(windowP, GLFW_KEY_RIGHT)) {
		cameraLong += static_cast<GLfloat>(cameraLongSpeed * timeDelta);
		if (cameraLong > glm::pi<GLfloat>() * 2) {
			cameraLong -= glm::pi<GLfloat>() * 2;
		}
		cam_moved = true;
	}
	if (glfwGetKey(windowP, GLFW_KEY_UP)) {
		cameraLati -= static_cast<GLfloat>(cameraLatiSpeed * timeDelta);
		if (cameraLati < 0.01f) {
			cameraLati = 0.01f;
		}
		cam_moved = true;
	}
	if (glfwGetKey(windowP, GLFW_KEY_DOWN)) {
		cameraLati += static_cast<GLfloat>(cameraLatiSpeed * timeDelta);
		if (cameraLati > glm::pi<GLfloat>() - 0.01f) {
			cameraLati = glm::pi<GLfloat>() - 0.01f;
		}
		cam_moved = true;
	}

	static bool leftBracket = false;
	if (glfwGetKey(windowP, GLFW_KEY_LEFT_BRACKET)) {
		if (leftBracket == false)
			leftBracket = true;
	}
	else if (leftBracket) {
		leftBracket = false;
		balls--;
		if (balls < 1)
			balls = 1;
	}

	static bool rightBracket = false;
	if (glfwGetKey(windowP, GLFW_KEY_RIGHT_BRACKET)) {
		if (rightBracket == false)
			rightBracket = true;
	}
	else if (rightBracket) {
		rightBracket = false;
		balls++;
		if (balls > 9)
			balls = 9;
	}
}

void ProjRaytracer::updateFPSonTitle()
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

void makePlantTexture(const char * path, int id)
{
	GLuint tex;
	glGenTextures(1, &tex);
	GLImage *image = new GLImage(path);
	switch(id)
	{
	case 1:
		glActiveTexture(GL_TEXTURE1);
		break;
	case 2:
		glActiveTexture(GL_TEXTURE2);
		break;
	case 3:
		glActiveTexture(GL_TEXTURE3);
		break;
	case 4:
		glActiveTexture(GL_TEXTURE4);
		break;
	case 5:
		glActiveTexture(GL_TEXTURE5);
		break;
	case 6:
		glActiveTexture(GL_TEXTURE6);
		break;
	case 7:
		glActiveTexture(GL_TEXTURE7);
		break;
	case 8:
		glActiveTexture(GL_TEXTURE8);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->imageBuf);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void ProjRaytracer::makeRectangle()
{
	// 在纹理中，用(s,t)来表示纹理贴图的二维坐标
	GLfloat vertices[] = {
		/*	X		Y		Z   ||		R		G	B		||		Texture-s	Texture-t*/
			-0.5f,	-0.5f,0.f,/*||*/	1.0f, 0.0f, 0.0f, /*||*/	0.0f,		0.0f,
			0.5f,	0.5f, 0.f,/*||*/	0.0f, 0.0f, 1.0f, /*||*/	1.0f,		1.0f,
			0.5f,	-0.5f,0.f,/*||*/	0.0f, 1.0f, 0.0f, /*||*/	1.0f,		0.0f,
			-0.5f,	0.5f, 0.f,/*||*/	1.0f, 1.0f, 1.0f, /*||*/	0.0f,		1.0f,
	};

	GLuint indices[] = {
		0, 1, 2,
		0, 3, 1
	};

	VAOWithDrawParam obj;
	glGenVertexArrays(1, &obj.vao);
	obj.mode = GL_TRIANGLES;
	obj.count = 6;	//两个三角形拼成一个矩形, 共6个顶点
	obj.type = GL_UNSIGNED_INT;
	obj.indices = nullptr;
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	/*int tex = glGetUniformLocation(programs[1]->id, "mytexture");
	glUniform1i((GLint)tex, 1);*/


	makePlantTexture("plant1.jpg", 1);
	makePlantTexture("plant2.jpg", 2);
	makePlantTexture("plant3.jpg", 3);
	makePlantTexture("plant4.jpg", 4);
	makePlantTexture("plant5.jpg", 5);
	makePlantTexture("plant6.jpg", 6);
	makePlantTexture("plant7.jpg", 7);
	makePlantTexture("plant8.jpg", 8);







}

void ProjRaytracer::makeTexture()
{
	glGenTextures(1, &sceneTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindImageTexture(0, sceneTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void ProjRaytracer::checkWorkingGroup()
{
	int sizeX, sizeY, sizeZ;
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &sizeX);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &sizeY);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &sizeZ);

	logFileP->log("Maximum work group sizes 最大工作组大小: %d x %d x %d = %d", sizeX, sizeY, sizeZ, sizeX * sizeY * sizeZ);

	int workingGroupInvocations;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workingGroupInvocations);

	logFileP->log("Maximum work group invocations(volumn) 最大工作组调用数(容量): %d", workingGroupInvocations);
}

void ProjRaytracer::makeMatrices()
{
	const GLfloat transArr[16] = {
		1.0f, 0.0f, 0.0f, 0.0f, // first column 
		0.0f, 1.0f, 0.0f, 0.0f, // second column 
		0.0f, 0.0f, 1.0f, 0.0f, // third column 
		0.0f, 0.0f, 0.0f, 1.0f // fourth column 
	};


	Model = glm::make_mat4(transArr);

	//View = glm::lookAt(glm::vec3(0.0f, 0.0f, 50.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0, 1, 0));
	View = glm::mat4(1);

	//Projection = glm::mat4(1);
	
	//Projection = glm::perspective(glm::radians(45.0f), 1.0f / 1.0f, 0.3f, 1000.0f);
}

void ProjRaytracer::progDrawObjects(GLuint progId)
{
	GLint modelLoc = glGetUniformLocation(progId, "Model");
	GLint viewLoc = glGetUniformLocation(progId, "View");
	GLint projectionLoc = glGetUniformLocation(progId, "Projection");

	
	Projection = glm::ortho(-0.5f, -0.5f + float(width) / initialWidth, -0.5f, -0.5f + float(height) / initialHeight, 0.0f, 1.0f);

	glUniformMatrix4fv(modelLoc, 1, false, glm::value_ptr(Model));
	glUniformMatrix4fv(viewLoc, 1, false, glm::value_ptr(View));
	glUniformMatrix4fv(projectionLoc, 1, false, glm::value_ptr(Projection));

	for (auto p = vaos.begin(); p != vaos.end(); p++)
	{
		glBindVertexArray(p->vao);
		glDrawElements(p->mode, p->count, p->type, p->indices);
	}
	glBindVertexArray(0);
}

void ProjRaytracer::progRaytrace(GLuint progId)
{
	GLuint paintNumLoc = glGetUniformLocation(progId, "paintNum");
	glUniform1i(paintNumLoc, this->balls);
	if (rayTimer >= RayComputePeriod)
	{
		GLint widthLoc = glGetUniformLocation(progId, "WIDTH");
		GLint heightLoc = glGetUniformLocation(progId, "HEIGHT");
		GLuint timeLoc = glGetUniformLocation(progId, "time");
		//GLuint rayDirectionLoc = glGetUniformLocation(progId, "rayDirection");
		GLuint rayViewingPointCenterLoc = glGetUniformLocation(progId, "rayViewingPoint_center");
		GLuint rayViewingPointEastLoc = glGetUniformLocation(progId, "rayViewingPoint_east");
		GLuint rayViewingPointNorthLoc = glGetUniformLocation(progId, "rayViewingPoint_north");
		GLuint cameraScaleLoc = glGetUniformLocation(progId, "CAMERASCALE");

		glm::vec3 cameraPosition = { 1000.0 * glm::sin(cameraLati) * glm::cos(cameraLong), 1000.0 * glm::sin(cameraLati) * glm::sin(cameraLong), 1000.0 * glm::cos(cameraLati) };
		//glm::vec3 rayDirection = -glm::normalize(cameraPosition);
		glm::vec3 cameraEast = glm::normalize(glm::cross(glm::vec3{ 0.0f, 0.0f, 1.0f }, cameraPosition));
		glm::vec3 cameraNorth = glm::normalize(glm::cross(cameraPosition, cameraEast));
		
		glUniform1iv(widthLoc, 1, &this->width);
		glUniform1iv(heightLoc, 1, &this->height);
		glUniform1f(timeLoc, (GLfloat)currTime);
		glUniform1f(cameraScaleLoc, glm::pow(10.0f, cameraScaleLog));
		//glUniform3fv(rayDirectionLoc, 1, glm::value_ptr(rayDirection));
		glUniform3fv(rayViewingPointCenterLoc, 1, glm::value_ptr(cameraPosition));
		glUniform3fv(rayViewingPointEastLoc, 1, glm::value_ptr(cameraEast));
		glUniform3fv(rayViewingPointNorthLoc, 1, glm::value_ptr(cameraNorth));

		rayTimer = 0;
		glDispatchCompute(width, height, 1);
	}
}

void ProjRaytracer::doInit()
{
	doGLFWInit();
	doCreateWindow();
	doGLEWInit();
	printGLParams(logFileP);
	doGLConfigure();
	doSetViewport();
}

void ProjRaytracer::run()
{
	doInit();
	doMakePrograms();
	makeTexture();
	makeRectangle();
	makeMatrices();
	checkWorkingGroup();
	mainLoop();
}


ProjRaytracer::~ProjRaytracer()
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
