#pragma once

#include "..\types.h"

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
	static uint64_t s_Frequency;

	uint64_t m_StartStamp;
	uint64_t m_StopStamp;
};

///////////////////////////////////////////////////////
//
// INLINES:
//
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
inline double HighResolutionTimer::ElapsedTimeInMiliseconds()
{
	return ElapsedTimeInSeconds() * 1000;
}