#pragma once

// �洢һ���� EBO �� VAO �Լ��� glDrawElements ����������Ҫ��������Ϣ��
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
