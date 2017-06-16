#include <gtest\gtest.h>
#include <array>
#include <sts\tools\ParallelFor.h>

typedef std::array<int, 16 > TArray16;
typedef std::array<int, 8 > TArray8;

namespace helpers
{
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

		sts::ParallelForEach( testArray.begin(), testArray.end(),
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
		sts::TaskManager manager;
		manager.Initialize();

		sts::ParallelForEachUsingTasks( testArray.begin(), testArray.end(),
			[]( TArray::iterator& it )
		{
			helpers::CalculateBigSum( it );
		}, manager );

		ASSERT_TRUE( helpers::ChcekIfEqual( testArray.begin(), testArray.end() ) );

		manager.Deinitialize();
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