#include "SimpleProfiler.h"
#include <assert.h>
#include <time.h>
#include <stdint.h>

#define TARGET_PLATFORM_WIN32 0
#define TARGET_PLATFORM_APPLE 1
#define TARGET_PLATFORM_LINUX 2

#ifdef _WIN32
	#define TARGET_PLATFORM TARGET_PLATFORM_WIN32
#elif __APPLE__
	#define TARGET_PLATFORM TARGET_PLATFORM_APPLE
#elif __linux__
	#define TARGET_PLATFORM TARGET_PLATFORM_LINUX
#else
#   error "Unknown compiler"

#endif



#if TARGET_PLATFORM == TARGET_PLATFORM_WIN32
#include <Windows.h>
#include <stdio.h>
#elif TARGET_PLATFORM == TARGET_PLATFORM_LINUX
#include <sys/time.h>
#include <stdarg.h>
#include <stdio.h>
#elif TARGET_PLATFORM == TARGET_PLATFORM_APPLE
#include <mach/mach_time.h>
#include <stdarg.h>
#include <stdio.h>
#endif

ProfileEntry SimpleProfiler::m_entries[SimpleProfiler::MAX_ENTRIES];
int SimpleProfiler::m_entryNum = 0;

float SimpleProfiler::getTime()
{
	int64_t time;
	
#if TARGET_PLATFORM == TARGET_PLATFORM_WIN32
	QueryPerformanceCounter((LARGE_INTEGER*)&time);

#elif TARGET_PLATFORM == TARGET_PLATFORM_APPLE
	time = mach_absolute_time();

#else
	struct timeval t_current;
	gettimeofday(&t_current, NULL);
	time = ((long)t_current.tv_sec) * 1000000 + (long)t_current.tv_usec;
#endif

	static double counterFreq = 0;
	static int64_t initialTickCount = 0;
	if (counterFreq == 0)
	{
		initialTickCount = time;

#if TARGET_PLATFORM == TARGET_PLATFORM_WIN32
		int64_t freq;
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		counterFreq = 1.0 / double(freq);
#elif TARGET_PLATFORM == TARGET_PLATFORM_APPLE
		mach_timebase_info_data_t sTimebaseInfo;
		mach_timebase_info(&sTimebaseInfo);
		counterFreq = (double)(sTimebaseInfo.numer) / sTimebaseInfo.denom / 1000000000;
#else
		counterFreq = 1.0 / 1000000.0;//1.0;
#endif

		counterFreq *= 1000;  //convert s to ms
	}
	time = time - initialTickCount;

	double miliseconds = double(time) * counterFreq;
	return (float)miliseconds;
}

ProfileEntry *SimpleProfiler::createEntry(const char *name)
{
	assert(m_entryNum < MAX_ENTRIES);  //not enough entries, set MAX_ENTRIES to a bigger value

	ProfileEntry *e = &m_entries[m_entryNum++];
	e->name = name;
	reset(e);
	return e;
}

void SimpleProfiler::resetAll()
{
	for (int i = 0; i < m_entryNum; i++)
		reset(&m_entries[i]);
}

void debugPrintWithTag(const char* tag, const char *format, ...)
{
	va_list argptr;

#if TARGET_PLATFORM == TARGET_PLATFORM_WIN32
	char tmp[1024];
	va_start(argptr, format);
	vsnprintf(tmp, 1024, format, argptr);
	va_end(argptr);
	OutputDebugString(tmp);
	OutputDebugString("\n");
#elif TARGET_PLATFORM == TARGET_PLATFORM_LINUX
	va_start(argptr, format);
	vprintf(format, argptr);
	va_end(argptr);
	printf("\n");
#else
	va_start(argptr, format);
	vprintf(format, argptr);
	va_end(argptr);
	printf("\n");
#endif
}

void debugPrint(const char *format, ...)
{
	va_list argptr;

#if TARGET_PLATFORM == TARGET_PLATFORM_WIN32
	char tmp[1024];
	va_start(argptr, format);
	vsnprintf(tmp, 1024, format, argptr);
	va_end(argptr);
	OutputDebugString(tmp);
	OutputDebugString("\n");
#elif TARGET_PLATFORM == TARGET_PLATFORM_LINUX
	va_start(argptr, format);
	vprintf(format, argptr);
	va_end(argptr);
	printf("\n");
#else
	va_start(argptr, format);
	vprintf(format, argptr);
	va_end(argptr);
	printf("\n");
#endif
}

void SimpleProfiler::printAll()
{
	debugPrint("\nProfile result begin ========================");

	for (int i = 0; i < m_entryNum; i++)
	{
		ProfileEntry &e = m_entries[i];

		float avgTime = e.totalTime / e.calltimes;

		debugPrint("  %-32s %d calls, time: total=%f, avg=%f, max=%f", e.name, e.calltimes, e.totalTime, avgTime, e.maxTime);
	}

	debugPrint("Profile result end   ========================");
}
