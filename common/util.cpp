#include "common.h"
using namespace std;

const char* GLtype_to_string(GLenum type) {
	switch (type) {
	case GL_BOOL: return "bool";
	case GL_INT: return "int";
	case GL_FLOAT: return "float";
	case GL_FLOAT_VEC2: return "vec2";
	case GL_FLOAT_VEC3: return "vec3";
	case GL_FLOAT_VEC4: return "vec4";
	case GL_FLOAT_MAT2: return "mat2";
	case GL_FLOAT_MAT3: return "mat3";
	case GL_FLOAT_MAT4: return "mat4";
	case GL_SAMPLER_2D: return "sampler2D";
	case GL_SAMPLER_3D: return "sampler3D";
	case GL_SAMPLER_CUBE: return "samplerCube";
	case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow";
	default: break;
	}
	return "other";
}

void printAllProgramInfo(GLuint id) {
	printf("--------------------\nshader id %i info:\n", id);
	int params = -1;
	glGetProgramiv(id, GL_LINK_STATUS, &params);
	printf("GL_LINK_STATUS = %i\n", params);

	glGetProgramiv(id, GL_ATTACHED_SHADERS, &params);
	printf("GL_ATTACHED_SHADERS = %i\n", params);

	glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &params);
	printf("GL_ACTIVE_ATTRIBUTES = %i\n", params);
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
				printf("  %i) type:%s name:%s location:%i\n",
					i, GLtype_to_string(type), long_name, location);
			}
		}
		else {
			int location = glGetAttribLocation(id, name);
			printf("  %i) type:%s name:%s location:%i\n",
				i, GLtype_to_string(type), name, location);
		}
	}

	glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &params);
	printf("GL_ACTIVE_UNIFORMS = %i\n", params);
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
				printf("  %i) type:%s name:%s location:%i\n",
					i, GLtype_to_string(type), long_name, location);
			}
		}
		else {
			int location = glGetUniformLocation(id, name);
			printf("  %i) type:%s name:%s location:%i\n",
				i, GLtype_to_string(type), name, location);
		}
	}
}


void printShaderLog(LogFile *_logFileP, GLuint sh)
{
	GLsizei actualLen = 0;
	char shaderLog[logLen];
	glGetShaderInfoLog(sh, logLen, &actualLen, shaderLog);
	_logFileP->error("Shader info log for GL index %u:\n%s\n", sh, shaderLog);
}

void printProgramLog(LogFile *_logFileP, GLuint shProg)
{
	GLsizei actualLen = 0;
	char progLog[logLen];
	glGetProgramInfoLog(shProg, logLen, &actualLen, progLog);
	_logFileP->error("Program info log for GL index %u:\n%s\n", shProg, progLog);
}

void printGLParams(LogFile *_logFileP)
{
	GLenum params[] = {
	GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
	GL_MAX_CUBE_MAP_TEXTURE_SIZE,
	GL_MAX_DRAW_BUFFERS,
	GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
	GL_MAX_TEXTURE_IMAGE_UNITS,
	GL_MAX_TEXTURE_SIZE,
	GL_MAX_VARYING_FLOATS,
	GL_MAX_VERTEX_ATTRIBS,
	GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
	GL_MAX_VERTEX_UNIFORM_COMPONENTS,
	GL_MAX_VIEWPORT_DIMS,
	GL_STEREO,
	};
	const char* names[] = {
	  "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
	  "GL_MAX_CUBE_MAP_TEXTURE_SIZE",
	  "GL_MAX_DRAW_BUFFERS",
	  "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
	  "GL_MAX_TEXTURE_IMAGE_UNITS",
	  "GL_MAX_TEXTURE_SIZE",
	  "GL_MAX_VARYING_FLOATS",
	  "GL_MAX_VERTEX_ATTRIBS",
	  "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
	  "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
	  "GL_MAX_VIEWPORT_DIMS",
	  "GL_STEREO",
	};

	lg("GL Context Params:\n");

	// integers - only works if the order is 0-10 integer return types
	for (int i = 0; i < 10; i++) {
		int v = 0;
		glGetIntegerv(params[i], &v);
		lg("%s %i", names[i], v);
	}
	// others
	int v[2];
	v[0] = v[1] = 0;
	glGetIntegerv(params[10], v);
	lg("%s %i %i", names[10], v[0], v[1]);
	unsigned char s = 0;
	glGetBooleanv(params[11], &s);
	lg("%s %u", names[11], (unsigned int)s);
	lg("-----------------------------");
}
