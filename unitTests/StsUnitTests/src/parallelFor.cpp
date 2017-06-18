#include <gtest\gtest.h>
#include <array>

#include "..\..\libs\simpleTaskSystem\include\tools\parallelFor.h"
#include "..\..\libs\simpleTaskSystem\include\globalApi.h"

#include "..\..\libs\basicThreadingLib\include\tools\tools.h"
#include "..\..\libs\basicThreadingLib\include\thread\functorThread.h"

typedef std::array<int, 16 > TArray16;
typedef std::array<int, 8 > TArray8;

namespace helpers
{
	///////////////////////////////////////////////////////////////////////////////////////////
	// Added as a comparsion to version based on tasks.
	template< class Iterator, typename Functor > 
	void ParallelForEachUsingThreads( const Iterator& begin, const Iterator& end, const Functor& functor, uint32_t max_num_of_threads )
	{
		if( max_num_of_threads == 0 )
			max_num_of_threads = btl::GetLogicalCoresSize();

		auto con_size = std::distance( begin, end );;
		auto batch_size = ( con_size / max_num_of_threads );
		Iterator last_it = end;

		std::vector< btl::FunctorThread > workers;
		std::vector<std::function<void( void )> > functors;

		// Setup job.
		// Split the for amoung requested number of threads (inlcuding thread that called that function):
		for( uint32_t i = 0; i < max_num_of_threads - 1; ++i )
		{
			Iterator start_it = begin;
			std::advance( start_it, i * batch_size );

			Iterator end_it = start_it;
			std::advance( end_it, batch_size );

			last_it = end_it;

			std::function< void( void ) > func = [ &functor, start_it, end_it ]()
			{
				for( auto it = start_it; it != end_it; ++it )
					functor( it );
			};

			functors.emplace_back( std::move( func ) );
		}

		// Start job. Start workers.
		for( auto& func : functors )
			workers.emplace_back( btl::FunctorThread() );

		for( uint32_t i = 0; i < max_num_of_threads - 1; ++i )
		{
			workers[ i ].SetFunctorAndStartThread( functors[ i ] );
			workers[ i ].SetThreadName( "ParallelFor_WorkerThread" );
		}

		// Do the rest job in this thread:
		for( auto it = last_it; it != end; ++it )
			functor( it );

		// Wait for thread to finish their jobs.
		for( auto& thread : workers )
			thread.Join();
	}

	//////////////////////////////////////////////////////////////////////
	// WARNING! This method has to take iterator and fill it, otherwise 
	// it will be optimaized by compiler in release( in case that it 
	// simply returns computed value ) for for-loop.
	template< class TIter >
	static void CalculateBigSum( TIter& it ) 
	{
		static const int IterationNumberBig = 1000000000;
		static const int IterationNumberSmall = 100000000;

		int sum = 0;
		for( int i = 0; i < IterationNumberSmall; ++i )
		{
			sum += ( i % 4 ) / 2;
		}

		*it = sum;
	}

	//////////////////////////////////////////////////////////////////////
	template < class TIter >
	static bool ChcekIfEqual( const TIter& begin, const TIter& end )
	{
		auto val = *begin;
		for( auto it = begin; it != end; ++it )
			if( *it != val )
				return false;
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////
	template< class TArray, unsigned NumOfThreads >
	void TestParallelForUsingThreads()
	{
		TArray testArray;

		ParallelForEachUsingThreads( testArray.begin(), testArray.end(),
			[]( TArray::iterator& it )
		{
			helpers::CalculateBigSum( it );
		}, NumOfThreads );

		ASSERT_TRUE( helpers::ChcekIfEqual( testArray.begin(), testArray.end() ) );
	}

	/////////////////////////////////////////////////////////////////////////////
	template< class TArray >
	void TestParallelForUsingTasks()
	{
		TArray testArray;
		auto manager = CreateTaskSystem();

		sts::tools::ParallelForEach( testArray.begin(), testArray.end(),
			[]( TArray::iterator& it )
		{
			helpers::CalculateBigSum( it );
		}, manager );

		ASSERT_TRUE( helpers::ChcekIfEqual( testArray.begin(), testArray.end() ) );

		DestroyTaskSystem( manager );
	}

	/////////////////////////////////////////////////////////////////////////////
	template< class TArray >
	void TestSerialForLoop()
	{
		TArray testArray;

		for( auto it = testArray.begin(); it != testArray.end(); ++it )
			helpers::CalculateBigSum( it );

		ASSERT_TRUE( helpers::ChcekIfEqual( testArray.begin(), testArray.end() ) );
	}
}

//////////////////////////////////////////////////////////////////////
TEST(STSParallelFor, SingleThread_8_Test )
{
	helpers::TestSerialForLoop< TArray8 >();
}

//////////////////////////////////////////////////////////////////////
TEST( STSParallelFor, MultiThreadUsing2Threads_8_Test )
{
	helpers::TestParallelForUsingThreads< TArray8, 2 >();
}

//////////////////////////////////////////////////////////////////////
TEST( STSParallelFor, MultiThreadUsing4Threads_8_Test )
{
	helpers::TestParallelForUsingThreads< TArray8, 4 >();
}

//////////////////////////////////////////////////////////////////////
TEST( STSParallelFor, MultiThreadUsing8Threads_8_Test )
{
	helpers::TestParallelForUsingThreads< TArray8, 8 >();
}

//////////////////////////////////////////////////////////////////////
TEST( STSParallelFor, MultiThreadUsingTasks_8_Test )
{
	helpers::TestParallelForUsingTasks< TArray8 >();
}

//////////////////////////////////////////////////////////////////////
TEST( STSParallelFor, SingleThread_16_Test )
{
	helpers::TestSerialForLoop< TArray16 >();
}

//////////////////////////////////////////////////////////////////////
TEST( STSParallelFor, MultiThreadUsing2Threads_16_Test )
{
	helpers::TestParallelForUsingThreads< TArray16, 2 >();
}

//////////////////////////////////////////////////////////////////////
TEST( STSParallelFor, MultiThreadUsing4Threads_16_Test )
{
	helpers::TestParallelForUsingThreads< TArray16, 4 >();
}

//////////////////////////////////////////////////////////////////////
TEST( STSParallelFor, MultiThreadUsing8Threads_16_Test )
{
	helpers::TestParallelForUsingThreads< TArray16, 8 >();
}

//////////////////////////////////////////////////////////////////////
TEST( STSParallelFor, MultiThreadUsingTasks_16_Test )
{
	helpers::TestParallelForUsingTasks< TArray16 >();
}