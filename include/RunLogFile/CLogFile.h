#ifndef __CLOGFILE_H__
#define __CLOGFILE_H__

#define _MAX_PATH 260

#define FA_LOG_TRACE		0
#define FA_LOG_DEBUG		10000
#define FA_LOG_INFO		20000
#define FA_LOG_WARN		30000
#define FA_LOG_ERROR		40000
#define FA_LOG_FATAL		50000
#define FA_LOG_OFF			100000

#ifdef __cplusplus
extern "C" {
#endif

void SetConsole(bool bConsole);

void SetLogFileName(const char* logname);

void iWriteRunLog(const char* file, int line, int level,  const char* format, ...);

#define WriteRunLog(level, format, ...) iWriteRunLog(__FILE__, __LINE__, level, format, ##__VA_ARGS__)

#ifdef __cplusplus
}
;
#endif

#endif