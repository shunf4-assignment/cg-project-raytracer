#pragma once
#define logFileP (&(LogFile::getInstance()))
#define lg logFileP->log
#define le logFileP->error
#define logPath "mylog.log"
const GLsizei logLen = 4096;

#define MY_LOG_MAXBUF 100
class LogFile
{
public:
	static LogFile &getInstance()
	{
		static LogFile theLogFile(logPath);
		return theLogFile;
	}

	std::string fileName;
	std::ofstream getFile(int mode);
	std::FILE * getFile(const char *mode);

public:
	LogFile(const char *fileName);
	void restartLogging();
	void log(const char *message, ...);
	void error(const char *message, ...);
	void error(const char *message, va_list args);
};

class ShaderFile
{
	std::stringstream text;
public:
	ShaderFile(const char *filePath);
	ShaderFile(const ShaderFile &sf);
	const std::string getText() const;
	~ShaderFile();
};

#define MY_EXCEPTION_MAXBUF 100
class MyGLException : std::exception
{
public:
	MyGLException(const char *message, ...)
	{
		char finalMessage[MY_EXCEPTION_MAXBUF];
		va_list args;
		va_start(args, message);
		vsprintf_s(finalMessage, message, args);
		va_end(args);
		std::cerr << finalMessage << std::endl;
		this->std::exception::exception(finalMessage);
	}

	MyGLException(LogFile &lf, const char *message, ...)
	{
		char finalMessage[MY_EXCEPTION_MAXBUF];
		va_list args;
		va_start(args, message);
		vsprintf_s(finalMessage, message, args);
		try
		{
			lf.error(message, args);
		}
		catch (MyGLException &e)
		{
			va_end(args);
			throw e;
		}
		va_end(args);
		this->std::exception::exception(finalMessage);
	}
};
