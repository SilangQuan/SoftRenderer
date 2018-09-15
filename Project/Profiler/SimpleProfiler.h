#pragma once





struct ProfileEntry
{
	unsigned int calltimes;
	float totalTime;
	float maxTime;

	float startTime;

	const char *name;
};

void debugPrint(const char *format, ...);
void debugPrintWithTag(const char* tag, const char *format, ...);

class SimpleProfiler
{
public:	
	static void reset(ProfileEntry *e)
	{
		e->calltimes = 0;
		e->totalTime = 0;
		e->maxTime = 0;
	}

	static void startCall(ProfileEntry *e)
	{
		e->startTime = getTime();
	}

	static void endCall(ProfileEntry *e)
	{
		float endTime = getTime();
		float dura = endTime - e->startTime;
		e->calltimes++;
		e->totalTime += dura;
		float maxTime = e->maxTime;
		e->maxTime = (dura > maxTime) ? dura : maxTime;
	}

	enum
	{
		MAX_ENTRIES = 64
	};

	static ProfileEntry m_entries[MAX_ENTRIES];
	static int m_entryNum;

	static float getTime();

	static ProfileEntry *createEntry(const char *name);

	static void resetAll();

	static void printAll();
};

#define PROFILE_BEGIN(name) \
	static ProfileEntry *_ProfileEntry_##name = SimpleProfiler::createEntry(#name); \
	SimpleProfiler::startCall(_ProfileEntry_##name);

#define PROFILE_END(name) \
	SimpleProfiler::endCall(_ProfileEntry_##name);

#define PROFILE_SCOPE(name) \
	static ProfileEntry *_ProfileEntry_##name = SimpleProfiler::createEntry(#name); \
	struct ScopeProfiler \
	{ \
		ProfileEntry *e; \
		ScopeProfiler(ProfileEntry * e) :e(e) { SimpleProfiler::startCall(e); } \
		~ScopeProfiler() { SimpleProfiler::endCall(e); } \
	} _scopeProfiler(_ProfileEntry_##name);
