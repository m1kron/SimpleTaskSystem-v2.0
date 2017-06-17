#include "..\..\include\timer\highResolutionTimer.h"
#include "..\..\include\macros.h"

#include "windows.h"


uint64_t HighResolutionTimer::s_Frequency = 0;

///////////////////////////////////////////////////////
HighResolutionTimer::HighResolutionTimer()
	: m_StartStamp( 0 )
	, m_StopStamp( 0 )
{
	LARGE_INTEGER freq;
	::QueryPerformanceFrequency( &freq );
	s_Frequency = freq.QuadPart;
}

///////////////////////////////////////////////////////
void HighResolutionTimer::Start()
{
	LARGE_INTEGER stamp;
	::QueryPerformanceCounter( &stamp );
	m_StartStamp = stamp.QuadPart;
}

///////////////////////////////////////////////////////
void HighResolutionTimer::Stop()
{
	LARGE_INTEGER stamp;
	::QueryPerformanceCounter( &stamp );
	m_StopStamp = stamp.QuadPart;
}

///////////////////////////////////////////////////////
void HighResolutionTimer::Reset()
{
	m_StartStamp = 0;
	m_StopStamp = 0;
}

///////////////////////////////////////////////////////
double HighResolutionTimer::ElapsedTimeInSeconds()
{
	ASSERT( m_StartStamp != 0 );
	ASSERT( m_StopStamp != 0 );
	ASSERT( s_Frequency != 0 );

	uint64_t diff;
	diff = m_StopStamp - m_StartStamp;

	double ret = ( (double)diff / (double)s_Frequency );
	return ret;
}