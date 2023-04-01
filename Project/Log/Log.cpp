#include "Log.h"


Debug MessageLogger::debug() const
{
	std::string debug = "debug";
	return Debug(&debug, Info, context);
}

Debug MessageLogger::info() const
{
	std::string debug = "info";
	return Debug(&debug, Info, context);
}

Debug MessageLogger::warning() const
{
	std::string debug = "warning";
	return Debug(&debug, Warning, context);
}

Debug MessageLogger::critical() const
{
	std::string debug = "error";
	return Debug(&debug, Warning, context);
}

