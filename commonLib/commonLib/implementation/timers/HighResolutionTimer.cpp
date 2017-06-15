#include <commonlib\timers\HighResolutionTimer.h>
#include <commonlib\Macros.h>


LARGE_INTEGER HighResolutionTimer::s_Frequency = LARGE_INTEGER();


///////////////////////////////////////////////////////
HighResolutionTimer::HighResolutionTimer()
	: m_StartStamp(LARGE_INTEGER())
	, m_StopStamp(LARGE_INTEGER())
{
	::QueryPerformanceFrequency(&s_Frequency);
}

///////////////////////////////////////////////////////
void HighResolutionTimer::Start()
{
	::QueryPerformanceCounter( &m_StartStamp );
}

///////////////////////////////////////////////////////
void HighResolutionTimer::Stop()
{
	::QueryPerformanceCounter( &m_StopStamp );
}

///////////////////////////////////////////////////////
void HighResolutionTimer::Reset()
{
	m_StartStamp = LARGE_INTEGER();
	m_StopStamp = LARGE_INTEGER();
}

///////////////////////////////////////////////////////
double HighResolutionTimer::ElapsedTimeInSeconds()
{
	ASSERT(m_StartStamp.QuadPart != 0);
	ASSERT(m_StopStamp.QuadPart != 0);
	ASSERT(s_Frequency.QuadPart != 0);

	LARGE_INTEGER diff;
	diff.QuadPart = m_StopStamp.QuadPart - m_StartStamp.QuadPart;

	double ret = ((double)diff.QuadPart / (double)s_Frequency.QuadPart);
	return ret;
}