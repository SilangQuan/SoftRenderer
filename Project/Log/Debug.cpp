#include "Debug.h"

Debug::~Debug()
{
	LogToConsole(stream->logType, stream->context, stream->ss.str());
	delete stream;
}

void Debug::LogToConsole(LogType type, const MessageLogContext &context, std::string logBuffer)
{
	std::string logString;
	switch (type)
	{
	case Error:
		logString.append("Error! ");
		break;
	case Info:
		//logString.append("");
		break;
	case Warning:
		logString.append("Warning! ");
		break;
	default:
		break;
	}
	logString.append(logBuffer);
	logString.append("......");

	logString.append(context.file);
	logString.append(" ");
	logString.append(context.function);
	logString.append("()");

	std::cout << logString <<" line: " << context.line << " "  << std::endl;

	//logString.append(context.line);
}