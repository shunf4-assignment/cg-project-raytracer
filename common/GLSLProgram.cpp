#include "common.h"
using namespace std;

GLSLProgram::GLSLProgram(std::initializer_list< GLSLShaderInput> shaderList)
{
	this->id = std::numeric_limits<GLuint>::max();
	this->shaders.clear();
	this->status = notInitialized;
	init(shaderList);
}

GLSLProgram::~GLSLProgram()
{
	for (auto p : this->shaders)
	{
		delete p;
	}
	if (id != std::numeric_limits<GLuint>::max())
		glDeleteProgram(id);
}

void GLSLProgram::init(const std::initializer_list< GLSLShaderInput> shaderList)
{
	this->shaders.clear();
	this->func = nullptr;
	this->status = notInitialized;

	try
	{
		for (auto p : shaderList)
		{
			auto sp = new GLSLShader(p);
			sp->compile();
			this->shaders.push_back(sp);
		}
	}
	catch (MyGLException &e)
	{
		this->status = error;
		throw e;
	}

	this->status = good;
	/*if (shaderList.size() > 0)
	{
		link();
	}
	else
	{
		this->status = notInitialized;
	}*/
}

GLuint GLSLProgram::link()
{
	switch (this->status)
	{
	case GLSLShaderStatus::notInitialized:
		throw MyGLException("Error: this GLSLProgram instance is not initialized with above one GLSLShader.");
	case GLSLShaderStatus::error:
		throw MyGLException("Error: this GLSLProgram instance is not in a good state.");
	case GLSLShaderStatus::compiled:
		return id;

	default:
		break;
	}

	int ivRes = -1;

	GLuint prog = glCreateProgram();
	id = prog;
	for (auto p : shaders)
	{
		if (p->status != compiled)
			throw MyGLException("Error: this GLSLShader instance is not compiled to be linked.");
		glAttachShader(prog, p->id);
	}

	glLinkProgram(prog);

	// Check error
	glGetProgramiv(prog, GL_LINK_STATUS, &ivRes);
	if (GL_TRUE != ivRes)
	{
		this->status = error;
		throw MyGLException("Error: GLSLProgram %u didn't link correctly.", prog);
	}
	else
	{
		this->status = compiled;
	}
	return id;
}

void GLSLProgram::validate()
{
	glValidateProgram(this->id);
	GLint params = -1;
	glGetProgramiv(this->id, GL_VALIDATE_STATUS, &params);
	if (GL_TRUE != params) {
		throw MyGLException("Error: GLSLProgram %u GL_VALIDATE_STATUS = GL_FALSE.", this->id);
	}
}

void GLSLProgram::printLog(LogFile *_logFileP)
{
	GLsizei actualLen = 0;
	char progLog[logLen / sizeof(char)];
	glGetProgramInfoLog(id, logLen, &actualLen, progLog);
	_logFileP->error("Program info log for GL index %u:\n%s\n", id, progLog);
}

void GLSLProgram::printInfo(LogFile *_logFileP)
{
	_logFileP->log("--------------------\nshader id %i info:\n", id);
	int params = -1;
	glGetProgramiv(id, GL_LINK_STATUS, &params);
	_logFileP->log("GL_LINK_STATUS = %i\n", params);

	glGetProgramiv(id, GL_ATTACHED_SHADERS, &params);
	_logFileP->log("GL_ATTACHED_SHADERS = %i\n", params);

	glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &params);
	_logFileP->log("GL_ACTIVE_ATTRIBUTES = %i\n", params);
	for (int i = 0; i < params; i++) {
		char name[64];
		int max_length = 64;
		int actual_length = 0;
		int size = 0;
		GLenum type;
		glGetActiveAttrib(
			id,
			i,
			max_length,
			&actual_length,
			&size,
			&type,
			name
		);
		if (size > 1) {
			for (int j = 0; j < size; j++) {
				char long_name[64];
				sprintf(long_name, "%s[%i]", name, j);
				int location = glGetAttribLocation(id, long_name);
				_logFileP->log("  %i) type:%s name:%s location:%i\n",
					i, GLtype_to_string(type), long_name, location);
			}
		}
		else {
			int location = glGetAttribLocation(id, name);
			_logFileP->log("  %i) type:%s name:%s location:%i\n",
				i, GLtype_to_string(type), name, location);
		}
	}

	glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &params);
	_logFileP->log("GL_ACTIVE_UNIFORMS = %i\n", params);
	for (int i = 0; i < params; i++) {
		char name[64];
		int max_length = 64;
		int actual_length = 0;
		int size = 0;
		GLenum type;
		glGetActiveUniform(
			id,
			i,
			max_length,
			&actual_length,
			&size,
			&type,
			name
		);
		if (size > 1) {
			for (int j = 0; j < size; j++) {
				char long_name[64];
				sprintf(long_name, "%s[%i]", name, j);
				int location = glGetUniformLocation(id, long_name);
				_logFileP->log("  %i) type:%s name:%s location:%i\n",
					i, GLtype_to_string(type), long_name, location);
			}
		}
		else {
			int location = glGetUniformLocation(id, name);
			_logFileP->log("  %i) type:%s name:%s location:%i\n",
				i, GLtype_to_string(type), name, location);
		}
	}
}
