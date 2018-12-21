#pragma once
struct GLSLShader
{
	GLuint id;
	GLenum type;
	GLSLShaderStatus status;
	ShaderFile *shaderFileP;

	GLSLShader(GLSLShaderInput &input);
	GLSLShader(GLenum type);
	GLSLShader(GLenum type, const char *shaderFilePath);
	GLSLShader(GLenum type, const ShaderFile &shaderFile);
	~GLSLShader();

	static GLuint proxyCreateShader(GLenum type);

	void loadShaderFile(const ShaderFile &shaderFile);
	GLuint compile();
	GLuint getId();
	void printLog(LogFile *);
};

class GLProject;
struct GLSLProgram
{
	GLuint id;
	GLSLShaderStatus status;
	std::vector< GLSLShader*> shaders;
	void (GLProject::*func)(GLuint progId);

	template <class GLProject_derived>
	GLSLProgram(void (GLProject_derived::*func)(GLuint progId), std::initializer_list< GLSLShaderInput> shaderList)
	{
		static_assert(std::is_base_of<GLProject, GLProject_derived>::value, "The class to which func belongs to is not derived from GLProject");
		this->GLSLProgram::GLSLProgram(shaderList);
		this->func = static_cast<void (GLProject::*)(GLuint)>(func);
	}

	GLSLProgram(std::initializer_list< GLSLShaderInput>);
	~GLSLProgram();

	void init(const std::initializer_list< GLSLShaderInput> shaderList);
	GLuint link();
	void validate();
	void printLog(LogFile *_logFileP);
	void printInfo(LogFile *_logFileP);
};


struct GLPixel
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

class GLImage
{
public:
	int w;
	int h;
	const int channel = 4;
	
	unsigned char *imageBuf;

	GLImage(const char *fileName);
	~GLImage();
	
	GLPixel *operator[](int i);
	GLPixel &get(int i);
	void reverse();
	void store(const char *fileName);
};

class GLProject
{
protected:
	std::vector < VAOWithDrawParam> vaos;
	std::vector < GLuint> buffers{};
	std::vector < GLSLProgram *> programs{};

	GLFWwindow *windowP;
	GLFWmonitor *monitorP;

	std::string windowTitle;

	int width;
	int height;
	bool isFullscreen;

protected:
	GLProject() {}
	static void keyCallbackFunc(GLFWwindow *window, int key, int scancode, int action, int mode);
	virtual void doInit() = 0;
	virtual ~GLProject() = 0;
	virtual void mainLoop() = 0;

public:
	virtual void run() = 0;
	virtual void setWidth(int width) { this->width = width; }
	virtual void setHeight(int height) { this->height = height; }

};