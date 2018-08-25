#include "Log.h"


Debug MessageLogger::debug() const
{
	std::string debug = "debug";
	Debug dbg = Debug(&debug);
	MessageLogContext &ctxt = dbg.stream->context;
	ctxt.copy(context);
	dbg.stream->logType = Info;
	return dbg;
}

Debug MessageLogger::info() const
{
	Debug dbg = Debug();
	MessageLogContext &ctxt = dbg.stream->context;
	ctxt.copy(context);
	dbg.stream->logType = Info;
	return dbg;
}

Debug MessageLogger::warning() const
{
	Debug dbg = Debug();
	MessageLogContext &ctxt = dbg.stream->context;
	ctxt.copy(context);
	dbg.stream->logType = Warning;
	return dbg;
}

Debug MessageLogger::critical() const
{
	Debug dbg = Debug();
	MessageLogContext &ctxt = dbg.stream->context;
	ctxt.copy(context);
	dbg.stream->logType = Error;
	return dbg;
}

