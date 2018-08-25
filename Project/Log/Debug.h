#pragma once

#include <iostream>  
#include <iomanip>  
#include <fstream>  
#include <string>  
#include <cstdlib>  
#include <stdint.h>  
#include <sstream>  
#include "Math/Vector2.h"  
#include "Math/Vector3.h"  
#include "Math/Quaternion.h"  
#include <vector>
//#include "Log.h"
#include "MessageLogContext.h"

enum LogType
{
	Info,
	Warning,
	Error,
	Default,
};

class Debug
{
public:
	struct Stream {
		Stream():ss(), space(true), context() {}
		Stream(std::string *s) :ss(""), space(true), context(){}
		std::ostringstream ss;
		bool space;
		MessageLogContext context;
		LogType logType;
	} *stream;

	Debug() : stream(new Stream()) {}
	inline Debug(std::string *s) : stream(new Stream(s)) {}
	~Debug();
	inline Debug &operator<<(bool t) { stream->ss<<(t ? "true" : "false"); return maybeSpace(); }
	inline Debug &operator<<(float t) { stream->ss << t; return maybeSpace(); }
	inline Debug &operator<<(int t) { stream->ss << t; return maybeSpace(); }
	inline Debug &operator<<(char t) { stream->ss << t; return maybeSpace(); }
	inline Debug &operator<<(signed short t) { stream->ss << t; return maybeSpace(); }
	inline Debug &operator<<(unsigned short t) { stream->ss << t; return maybeSpace(); }
	inline Debug &operator<<(std::string s) { stream->ss << s; return maybeSpace(); }
	inline Debug &operator<<(const char* c) { stream->ss << c; return maybeSpace(); }
	inline Debug &operator<<(Vector2 vec) { stream->ss << "(" << vec.x <<","<< vec.y<<")"; return maybeSpace(); }
	inline Debug &operator<<(Vector3 vec) { stream->ss << "(" << vec.x << "," << vec.y <<"," << vec.z << ")"; return maybeSpace(); }
	inline Debug &operator<<(Quaternion q) { stream->ss << "(" << q.x << "," << q.y << "," << q.z << "," << q.w << ")"; return maybeSpace(); }
	inline Debug &operator<<(Matrix4x4 m) { stream->ss << "(" << m[0] << "," << m[1] << "," << m[2] << "," << m[3]<< ";"
																<<m[4] << "," << m[5] << "," << m[6] << "," << m[7]<< ";"
																<<m[8] << "," << m[9] << "," << m[10] << "," << m[11]<< ";"
																<<m[12] << "," << m[13] << "," << m[14] << "," << m[15]<< ";"
																<<")"; return maybeSpace(); }

	inline Debug &space() { stream->space = true; stream->ss << ' '; return *this; }
	inline Debug &nospace() { stream->space = false; return *this; }
	inline Debug &maybeSpace() { if (stream->space) stream->ss << ' '; return *this; }

	template <typename T>
	inline Debug &operator<<(const std::vector<T> &vec)
	{
		stream->ss << '(';
		for (int i = 0; i < vec.size(); ++i) {
			stream->ss << vec.at(i);
			stream->ss << ", ";
		}
		stream->ss << ')';
		return maybeSpace();
	}

	void LogToConsole(LogType type, const MessageLogContext &context, std::string logBuffer);

private:
	static Debug* _instance;
};

