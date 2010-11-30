#include "log.h"

FileWriter::FileWriter(const char* path)
{
	_type = LOGFILE;
	_path = path;
}

FileWriter::~FileWriter()
{ cout<<"~~~~"<<endl; }

int FileWriter::Write(const char* data, unsigned int len)
{
	cout<<_path.c_str()<<endl;
	_fout.open(_path.c_str(), ios::app);//append模式
	_fout<<data;
	cout<<data<<endl;
	_fout.close();
	return 0;
}

void Log::setWriter(LogWriter* lw)
{ _writer = lw; }

void Log::setLevel(unsigned int level)
{ _level = level; }

void Log::setFormat(const char* fmt)
{ _format = fmt; }

void Log::logError(const char* fmt, ...)
{
	va_list marker;
	char buff[LOG_MAX_BUFF_SIZE];
	va_start(marker, fmt);
	vsprintf(buff, fmt, marker);
	va_end(marker);
	printString(LOG_ERROR, buff);
}

void Log::logDebug(const char* fmt, ...)
{
	va_list marker;
	char buff[LOG_MAX_BUFF_SIZE];
	va_start(marker, fmt);
	vsprintf(buff, fmt, marker);
	va_end(marker);
	printString(LOG_DEBUG, buff);
}

void Log::logWarning(const char* fmt, ...)
{
	va_list marker;
	char buff[LOG_MAX_BUFF_SIZE];
	va_start(marker, fmt);
	vsprintf(buff, fmt, marker);
	va_end(marker);
	printString(LOG_WARNING, buff);
}

void Log::logMessage(const char* fmt, ...)
{
	va_list marker;
	char buff[LOG_MAX_BUFF_SIZE];
	va_start(marker, fmt);
	vsprintf(buff, fmt, marker);
	va_end(marker);
	printString(LOG_MESSAGE, buff);
}

void Log::logPrint(unsigned int level, const char* fmt, ...)
{
	va_list marker;
	char buff[LOG_MAX_BUFF_SIZE];
	va_start(marker, fmt);
	vsprintf(buff, fmt, marker);
	va_end(marker);
	printString(level, buff);
}

void Log::printString(unsigned int level, const char* data)
{
	char* levelName;
	if (!(level & _level))
		return;

	switch (level) {
	case LOG_ERROR:
		levelName = "ERROR";
		break;
	case LOG_WARNING:
		levelName = "WARNING";
		break;
	case LOG_DEBUG:
		levelName = "DEBUG";
		break;
	case LOG_MESSAGE:
		levelName = "MESSAGE";
		break;
	default:
		return;
	}

	if (_format == "")
		return;

	std::string format;
	char* mark;
	char* occur;
	format = _format;
	int len;


	// 将"<TIME>\t<LEVEL>\t<MESSAGE>\n"中的对应段替换掉
	mark = "<TIME>";
	len = strlen(mark);
	if (occur = (char*)strstr(format.c_str(), (const char*)mark))
		format.replace(occur-format.c_str(), len, getTime());
	std::cout<<"11111"<<endl;

	mark = "<LEVEL>";
	len = strlen(mark);
	if (occur = (char*)strstr(format.c_str(), (const char*)mark))
		format.replace(occur-format.c_str(), len, levelName);
	std::cout<<"22222"<<endl;

	mark = "<MESSAGE>";
	len = strlen(mark);
	if (occur = (char*)strstr(format.c_str(), mark))
		format.replace(occur-format.c_str(), len, data);
	std::cout<<"333333"<<endl;

	_writer->Write(format.c_str(), format.size());
}

const char* Log::getTime()
{
	time_t timep;  
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	if(p == NULL)
		return NULL;
	sprintf(
			(char*)_time.c_str(), "%d-%d-%d %d:%d:%d", 
			p->tm_year+1900, p->tm_mon+1, 
			p->tm_mday, p->tm_hour,
			p->tm_min, p->tm_sec
	);
	return _time.c_str();//_time为类成员，该内存地址不会无效
}

/*
int main()
{
	FileWriter* fw = new FileWriter("log.txt");
	//FileWriter fw("log.txt");
	Log log;
	log.setWriter(fw);
	log.setLevel(LOG_DEBUG);
	log.setFormat();
	log.logDebug("we are the %d member", 5);
	delete fw;
	return 0;
}
*/
