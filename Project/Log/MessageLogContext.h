#pragma once
#include <string>

class MessageLogContext
{
public:
	MessageLogContext() : line(0), file(0), function(0) {}
	MessageLogContext(const char *fileName, const char *functionName, int lineNumber)
		: file(fileName), function(functionName), line(lineNumber) {}

	int line;
	const char *file;
	const char *function;
	void copy(const MessageLogContext &logContext)
	{
		this->file = logContext.file;
		this->line = logContext.line;
		this->function = logContext.function;
	}

private:
	friend class MessageLogger;
	friend class Debug;
};
