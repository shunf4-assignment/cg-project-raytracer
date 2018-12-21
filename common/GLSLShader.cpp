#include "common.h"
using namespace std;

GLSLShader::GLSLShader(GLSLShaderInput &input) : shaderFileP(nullptr)
{
	this->GLSLShader::GLSLShader(input.type, input.shaderFilePath);
}

GLSLShader::GLSLShader(GLenum type) : shaderFileP(nullptr)
{
	this->id = std::numeric_limits<GLuint>::max();
	this->type = type;
	this->status = notInitialized;
}

GLSLShader::GLSLShader(GLenum type, const char *shaderFilePath) : shaderFileP(nullptr)
{
	this->GLSLShader::GLSLShader(type);
	this->shaderFileP = new ShaderFile(shaderFilePath);
	this->status = good;
}

GLSLShader::GLSLShader(GLenum type, const ShaderFile &shaderFile) : shaderFileP(nullptr)
{
	this->GLSLShader::GLSLShader(type);
	this->shaderFileP = new ShaderFile(shaderFile);
	this->status = good;
	//this->compile();
}

void GLSLShader::loadShaderFile(const ShaderFile &shaderFile) 
{
	this->shaderFileP = new ShaderFile(shaderFile);
	this->status = good;
}

GLuint GLSLShader::compile()
{
	switch (this->status)
	{
	case GLSLShaderStatus::notInitialized:
		throw MyGLException("Error: this GLSLShader instance is not initialized with a filePath.");
	case GLSLShaderStatus::error:
		throw MyGLException("Error: this GLSLShader instance is not in a good state.");
	case GLSLShaderStatus::compiled:
		return id;

	default:
		break;
	}

	const string &shText = shaderFileP->getText();
	const char *shCStr = shText.c_str();

	int ivRes = -1;

	GLuint sh = glCreateShader(this->type);
	this->id = sh;
	glShaderSource(sh, 1, &shCStr, nullptr);
	glCompileShader(sh);

	// Check error
	glGetShaderiv(sh, GL_COMPILE_STATUS, &ivRes);
	if (GL_TRUE != ivRes)
	{
		this->status = error;
		this->printLog(logFileP);
		throw MyGLException("Error: GLSLShader %u didn't compile correctly.", sh);
	}
	else
	{
		this->status = compiled;
	}
	return id;
}

GLuint GLSLShader::getId()
{
	if (status != GLSLShaderStatus::compiled)
	{
		throw MyGLException("Error: this GLSLShader is not yet compiled in getId()");
	}
	return id;
}

void GLSLShader::printLog(LogFile *_logFileP)
{
	GLsizei actualLen = 0;
	char shaderLog[logLen / sizeof(char)];
	glGetShaderInfoLog(this->id, logLen, &actualLen, shaderLog);
	logFileP->error("Shader info log for GL index %u:\n%s\n", this->id, shaderLog);
}

GLSLShader::~GLSLShader()
{
	if (this->shaderFileP != nullptr)
	{
		delete shaderFileP;
	}

	if (id != std::numeric_limits<GLuint>::max())
		glDeleteShader(id);
}

GLuint GLSLShader::proxyCreateShader(GLenum type)
{
	switch (type)
	{
	case GL_VERTEX_SHADER:
		return glCreateShader(GL_VERTEX_SHADER);
	case GL_FRAGMENT_SHADER:
		return glCreateShader(GL_FRAGMENT_SHADER);
	default:
		return 0;
	}
}
