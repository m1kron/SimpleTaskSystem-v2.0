#pragma once

#include "..\tools\print.h"
#include "highResolutionTimer.h"


#ifndef PROFILING_DISABLED

#define PREPARE_PROFILING HighResolutionTimer timer;

#define PROFILE_LINE( line, comment_log )				\
	timer.Reset();										\
    timer.Start();										\
	line;												\
	timer.Stop();										\
	Print( "%s %f\n",timer.ElapsedTimeInSeconds() );	\
    

#define BEGIN_PROFILE_SECTION   \
	timer.Reset();              \
	timer.Start();

#define END_PROFILE_SECTION( comment_log )				\
	timer.Stop();										\
	Print( "%s %f\n",timer.ElapsedTimeInSeconds() );

#define ACCUMULATE_PROFILED_TIME( accumulator, num_of_tries )	\
	accumulator += timer.ElapsedTimeInSeconds();				\
	++num_of_tries;

#else
#define PREPARE_PROFILING
#define PROFILE_LINE( line, comment_log ) line
#define BEGIN_PROFILE_SECTION
#define END_PROFILE_SECTION( comment_log )
#define ACCUMULATE_PROFILED_TIME( accumulator, num_of_tries )
#endif

