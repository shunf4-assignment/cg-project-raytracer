#include "common.h"
using namespace std;

ShaderFile::ShaderFile(const char *filePath)
{
	ifstream file;
	file = ifstream(filePath);
	if (!file.is_open())
	{
		cerr << "Shader file " << filePath << " can't be open." << endl;
		return;
	}

	text << file.rdbuf();
	text.seekg(ios::beg, 0);
	file.close();
}

ShaderFile::ShaderFile(const ShaderFile & sf)
{
	this->text << sf.text.rdbuf();
}

const string ShaderFile::getText() const
{
	return text.str();
}

ShaderFile::~ShaderFile()
{
}
