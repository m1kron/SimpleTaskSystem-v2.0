#pragma once

#include <iostream>
#include <commonlib\timers\HighResolutionTimer.h>


#ifndef PROFILING_DISABLED

#define PREPARE_PROFILING HighResolutionTimer timer;

#define PROFILE_LINE( line, comment_log ) \
	timer.Reset();                  \
    timer.Start();                  \
	line;                           \
	timer.Stop();                   \
	std::cout << comment_log << timer.ElapsedTimeInSeconds() << std::endl; \
    

#define BEGIN_PROFILE_SECTION   \
	timer.Reset();              \
	timer.Start();

#define END_PROFILE_SECTION( comment_log ) \
	timer.Stop();                          \
	std::cout << comment_log << timer.ElapsedTimeInSeconds() << std::endl; 

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

