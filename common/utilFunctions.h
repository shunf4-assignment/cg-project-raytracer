#pragma once
// Utilities
const char* GLtype_to_string(GLenum type);
void printAllProgramInfo(GLuint programme);
void printShaderLog(LogFile *_logFileP, GLuint sh);
void printProgramLog(LogFile *_logFileP, GLuint sh);
void printGLParams(LogFile *_logFileP);
