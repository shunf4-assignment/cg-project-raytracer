#pragma once

// 存储一个有 EBO 的 VAO 以及用 glDrawElements 绘制它所需要的所有信息。
struct VAOWithDrawParam
{
	GLuint vao;
	GLenum mode;
	GLsizei count;
	GLenum type;
	const void *indices;
	glm::mat4 Model;
};

enum GLSLShaderStatus {
	notInitialized,
	good,
	compiled,
	error
};

struct GLSLShaderInput { GLenum type; const char *shaderFilePath; };
