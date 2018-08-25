#pragma once
class Time
{
public:
	static float deltaTime;
	static float time;
	static float fps;

	Time();
	~Time();
	void Start();
	void Stop();
	void Restart();
	void Update();

private:
	float startMark;
	float lastUpdateMark;
	int frameCountInOneSecond;
	float lastFpsMark;
};