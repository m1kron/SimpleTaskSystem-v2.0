#pragma once

#include <windows.h>

class HighResolutionTimer
{
public:
	HighResolutionTimer();

	void Reset();
	void Start();
	void Stop();
	double ElapsedTimeInMiliseconds();
	double ElapsedTimeInSeconds();

private:
	static LARGE_INTEGER s_Frequency;

	LARGE_INTEGER m_StartStamp;
	LARGE_INTEGER m_StopStamp;
};

///////////////////////////////////////////////////////
//
// INLINES:
//
///////////////////////////////////////////////////////

inline double HighResolutionTimer::ElapsedTimeInMiliseconds()
{
	return ElapsedTimeInSeconds() * 1000;
}