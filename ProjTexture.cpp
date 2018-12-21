#include "ProjTexture.h"

ProjTexture::ProjTexture()
{
	//this->GLProject::GLProject();
	this->height = 800;
	this->width = 1024;
	this->isFullscreen = false;
	this->windowTitle = "My Texture Program";
	this->image = new GLImage("test.png");
}



void ProjTexture::mainLoop()
{
	while (!glfwWindowShouldClose(windowP))
	{
		static double prevTime = glfwGetTime();
		double currTime = glfwGetTime();
		double timeDelta = currTime - prevTime;
		prevTime = currTime;

		glfwPollEvents();
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

void ProjTexture::keyCallbackFunc(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void ProjTexture::doGLConfigure()
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

void ProjTexture::doGLFWInit()
{
	logFileP->log("Starting GLFW %s", glfwGetVersionString());
	glfwSetErrorCallback([](int error, const char *desc) -> void {logFileP->error("GLFW Error: [%i] %s", error, desc); });

	if (!glfwInit())
	{
		throw MyGLException("Failed to initialize GLFW");
	}
}

void ProjTexture::doGLEWInit()
{
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		throw MyGLException("Failed to initialize GLEW");
	}
}

void ProjTexture::doCreateWindow()
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
	glfwSetKeyCallback(windowP, ProjTexture::keyCallbackFunc);
}

void ProjTexture::doSetViewport()
{
	glfwGetFramebufferSize(windowP, &width, &height);
	//glViewport(0, 0, width > height ? height : width, width > height ? height : width);
	glViewport(0, 0, width, height);
}

void ProjTexture::doMakePrograms()
{
	GLSLProgram *prog1 = nullptr;

	prog1 = new GLSLProgram(&ProjTexture::progDrawObjects, { {GL_VERTEX_SHADER, "projTexture.vert"}, {GL_FRAGMENT_SHADER, "projTexture.frag"} });

	prog1->link();
	for (auto p : prog1->shaders)
	{
		p->printLog(logFileP);
	}
	prog1->printInfo(logFileP);
	this->programs.push_back(prog1);
}

void ProjTexture::updateFPSonTitle()
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

void ProjTexture::makeRectangle()
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
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<const void *>(3 * sizeof(vertices[0])));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<const void *>(6 * sizeof(vertices[0])));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ProjTexture::makeTexture()
{
	GLuint tex;
	image->store("test-2.png");
	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->imageBuf);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void ProjTexture::makeMatrices()
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
	Projection = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, 0.0f, 1.0f);
	//Projection = glm::perspective(glm::radians(45.0f), 1.0f / 1.0f, 0.3f, 1000.0f);
}

void ProjTexture::progDrawObjects(GLuint progId)
{
	GLint modelLoc = glGetUniformLocation(progId, "Model");
	GLint viewLoc = glGetUniformLocation(progId, "View");
	GLint projectionLoc = glGetUniformLocation(progId, "Projection");

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

void ProjTexture::doInit()
{
	doGLFWInit();
	doCreateWindow();
	doGLEWInit();
	printGLParams(logFileP);
	doGLConfigure();
	doSetViewport();
}

void ProjTexture::run()
{
	doInit();
	doMakePrograms();
	makeTexture();
	makeRectangle();
	makeMatrices();
	mainLoop();
}


ProjTexture::~ProjTexture()
{
	delete image;

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
