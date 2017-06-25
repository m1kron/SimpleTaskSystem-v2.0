#pragma once

#include "..\types.h"

class HighResolutionTimer
{
public:
	HighResolutionTimer();

	// Start timer.
	void Start();

	// Resets timer.
	void Reset();
	
	// Returns elapsed time since start in miliseconds.
	double ElapsedTimeInMiliseconds();

	// Returns elapsed time since start in seconds.
	double ElapsedTimeInSeconds();

private:
	// Returns current timestamp.
	uint64_t GetTimeStamp();

	static uint64_t s_frequency;

	uint64_t m_startStamp;
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