#include "common.h"
#include <ctime>
#include <locale>
using namespace std;

LogFile::LogFile(const char *fileName) : fileName(fileName)
{
	restartLogging();
}

ofstream LogFile::getFile(int mode)
{
	ofstream file(fileName, mode);
	if (!file.is_open())
	{
		throw MyGLException("Error: Log file %s can't be open as write.", fileName);
	}
	return file;
}

FILE *LogFile::getFile(const char *mode)
{
	FILE *file = fopen(fileName.c_str(), mode);
	if (!file)
	{
		throw MyGLException("Error: Log file %s can't be open as write.", fileName);
	}
	return file;
}

void LogFile::log(const char *message, ...)
{
	va_list args;
	int result1, result2;
	FILE *file = getFile("a");
	va_start(args, message);
	vfprintf(stdout, message, args);
	fprintf(stdout, "\n");
	result1 = vfprintf_s(file, message, args);
	result2 = result1 < 0 ? -1 : fprintf_s(file, "\n");
	va_end(args);
	if (result2 < 0)
	{
		throw MyGLException("Error: Writing %s to %s failed.", message, fileName);
	}
	fclose(file);
}

void LogFile::error(const char *message, ...)
{
	va_list args;
	va_start(args, message);
	try
	{
		error(message, args);
	}
	catch (MyGLException &e)
	{
		va_end(args);
		throw e;
	}
	va_end(args);
}



void LogFile::error(const char *message, va_list args)
{
	int result1, result2;
	FILE *file = getFile("a");
	vfprintf(stderr, message, args);
	result1 = vfprintf_s(file, message, args);
	result2 = result1 < 0 ? -1 : fprintf_s(file, "\n");
	if (result2 < 0)
	{
		throw MyGLException("Error: Writing %s to %s failed.", message, fileName);
	}
	fclose(file);
}

void LogFile::restartLogging()
{
	time_t tt = time(NULL);
	char mbstr[100];
	
	strftime(mbstr, sizeof(mbstr), "%c", std::localtime(&tt));

	FILE *file = getFile("w");
	fprintf(file, "Logging - %s\n", mbstr);
	fclose(file);
}