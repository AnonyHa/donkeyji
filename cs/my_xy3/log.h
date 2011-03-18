#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#define LOG_MAX_BUFF_SIZE 1024*60

enum WriterType {
	LOGFILE,
	UNKNOWN
};

class LogWriter
{
protected:
	WriterType _type;
public:
	LogWriter() {
		_type = UNKNOWN;
	}

	virtual ~LogWriter()
	{}

	virtual int Write(const char* data, unsigned int len) {
		return 0;
	}

	WriterType getType() {
		return _type;
	}
};

class FileWriter : public LogWriter
{
private:
	std::string _path;
	std::ofstream _fout;
public:
	FileWriter(const char* path);
	~FileWriter();
	virtual int Write(const char* data, unsigned int len);
};

enum Level {
	LOG_ERROR = 0x01,
	LOG_WARNING = 0x02,
	LOG_MESSAGE = 0x04,
	LOG_DEBUG = 0x08,
	LOG_ALL = LOG_ERROR|LOG_WARNING|LOG_MESSAGE|LOG_DEBUG
};

class Log
{
private:
	std::string _format;
	std::string _time;
	unsigned int _level;
	LogWriter* _writer;
private:
	void printString(unsigned int level, const char* data);
	const char* getTime();
public:
	void setWriter(LogWriter* lw);
	void setLevel(unsigned int level);
	void setFormat(const char* fmt = "<TIME>\t<LEVEL>\t<MESSAGE>\n");

	void logError(const char* fmt, ...);
	void logDebug(const char* fmt, ...);
	void logWarning(const char* fmt, ...);
	void logMessage(const char* fmt, ...);
	void logPrint(unsigned int level, const char* fmt, ...);
};
