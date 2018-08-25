#pragma once
#define qDebug MessageLogger(__FILE__, __FUNCTION__, __LINE__).debug
#define qInfo MessageLogger(__FILE__, __FUNCTION__, __LINE__).info
#define qWarning MessageLogger(__FILE__, __FUNCTION__, __LINE__).warning
#define qCritical MessageLogger(__FILE__, __FUNCTION__, __LINE__).critical
#define qFatal MessageLogger(__FILE__, __FUNCTION__, __LINE__).fatal
#include "Debug.h"
#include "MessageLogContext.h"

class MessageLogger
{
public:
	MessageLogger() : context(){}
	MessageLogger(const char *fileName, const char *functionName, int lineNumber)
		: context(fileName, functionName, lineNumber) {}

	Debug info() const;
	Debug warning() const;
	Debug critical() const;
	Debug debug() const;

protected:
private:
	MessageLogContext context;
};

