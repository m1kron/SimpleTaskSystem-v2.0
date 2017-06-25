#include "..\..\include\timer\highResolutionTimer.h"
#include "..\..\include\macros.h"

#include "windows.h"


uint64_t HighResolutionTimer::s_frequency = 0;

///////////////////////////////////////////////////////
HighResolutionTimer::HighResolutionTimer()
	: m_startStamp( 0 )
{
	LARGE_INTEGER freq;
	::QueryPerformanceFrequency( &freq );
	s_frequency = freq.QuadPart;
}

///////////////////////////////////////////////////////
void HighResolutionTimer::Start()
{
	m_startStamp = GetTimeStamp();
}

///////////////////////////////////////////////////////
void HighResolutionTimer::Reset()
{
	m_startStamp = 0;
}

///////////////////////////////////////////////////////
double HighResolutionTimer::ElapsedTimeInSeconds()
{
	ASSERT( m_startStamp != 0 );
	ASSERT( s_frequency != 0 );

	uint64_t diff;
	diff = GetTimeStamp() - m_startStamp;

	double ret = ( (double)diff / (double)s_frequency );
	return ret;
}

///////////////////////////////////////////////////////
uint64_t HighResolutionTimer::GetTimeStamp()
{
	LARGE_INTEGER stamp;
	::QueryPerformanceCounter( &stamp );
	return stamp.QuadPart;
}
