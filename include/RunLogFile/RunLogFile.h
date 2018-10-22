#ifndef __CLOGFILE_H__
#define __CLOGFILE_H__

#define _MAX_PATH 260

#define LOG_TRACE		0
#define LOG_DEBUG		10000
#define LOG_INFO		20000
#define LOG_WARN		30000
#define LOG_ERROR		40000
#define LOG_FATAL		50000
#define LOG_OFF			100000

#ifdef __cplusplus
extern "C" {
#endif

	void SetConsole(bool bConsole);

	void SetLogFileName(const char* logname);

	void iWriteRunLog(const char* file, int line, int level, const char* format, ...);

#define WriteRunLog(level, format, ...) iWriteRunLog(__FILE__, __LINE__, level, format, ##__VA_ARGS__)

#ifdef __cplusplus
}
;
#endif

#endif